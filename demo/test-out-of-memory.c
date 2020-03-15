/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-out-of-memory.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include <foo.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct tracking_mem_context_s {
	unsigned long allocs;
	unsigned long alloc_bytes;
	unsigned long frees;
	unsigned long free_bytes;
	unsigned long fails;
	unsigned long max_used;
	unsigned long attempts;
	unsigned long attempts_to_fail_bitmask;
};

void *test_malloc(void *context, size_t size)
{
	struct tracking_mem_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	void *ptr = NULL;
	size_t used = 0;

	ptr = NULL;
	ctx = (struct tracking_mem_context_s *)context;
	if (0x01 & (ctx->attempts_to_fail_bitmask >> ctx->attempts++)) {
		return NULL;
	}
	tracking_buffer = malloc(sizeof(size_t) + size);
	if (!tracking_buffer) {
		++ctx->fails;
		return NULL;
	}

	memcpy(tracking_buffer, &size, sizeof(size_t));
	++ctx->allocs;
	ctx->alloc_bytes += size;
	if (ctx->free_bytes > ctx->alloc_bytes) {
		fflush(stdout);
		fprintf(stderr,
			"%s: %d BAD MOJO: free_bytes > alloc_bytes?! (%lu > %lu)\n",
			__FILE__, __LINE__, (unsigned long)ctx->free_bytes,
			(unsigned long)ctx->alloc_bytes);
	} else {
		used = ctx->alloc_bytes - ctx->free_bytes;
		if (used > ctx->max_used) {
			ctx->max_used = used;
		}
	}
	ptr = (void *)(tracking_buffer + sizeof(size_t));
	return ptr;
}

void test_free(void *context, void *ptr)
{
	struct tracking_mem_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	size_t size = 0;

	ctx = (struct tracking_mem_context_s *)context;
	if (ptr == NULL) {
		++ctx->fails;
		return;
	}
	tracking_buffer = ((unsigned char *)ptr) - sizeof(size_t);
	memcpy(&size, tracking_buffer, sizeof(size_t));
	ctx->free_bytes += size;
	++ctx->frees;
	free(tracking_buffer);
	if (ctx->free_bytes > ctx->alloc_bytes) {
		fflush(stdout);
		fprintf(stderr,
			"%s: %d BAD MOJO: free_bytes > alloc_bytes?! (%lu > %lu) just freed %lu\n",
			__FILE__, __LINE__, (unsigned long)ctx->free_bytes,
			(unsigned long)ctx->alloc_bytes, (unsigned long)size);
	}

}

#ifndef logerror
#define logerror(format, ...) \
        do { \
                fflush(stdout); /* avoid POSIX undefined stdout+stderr */ \
                /* https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */ \
                fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
                fprintf(stderr, format, __VA_ARGS__); \
                fprintf(stderr, "\n"); \
        } while (0)
#endif

int test_out_of_memory_loop(unsigned long malloc_fail_bitmask)
{
	int failures = 0;
	int err = 0;
	size_t i, loops;
	struct tracking_mem_context_s mctx;
	struct foo_s *foo;
	int rv_err;

	memset(&mctx, 0, sizeof(struct tracking_mem_context_s));
	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	foo = foo_new_custom_allocator(test_malloc, test_free, &mctx);
	if (!foo) {
		++err;
		if (!malloc_fail_bitmask) {
			logerror("new returned NULL, but bitmask: %lu",
				 malloc_fail_bitmask);
			++failures;
		}
		goto end_test_out_of_memory;
	}

	loops = 5;
	for (i = 0; i < 5; ++i) {
		rv_err = foo_a(foo);
		if (rv_err) {
			++err;
			if (!malloc_fail_bitmask) {
				logerror
				    ("foo_a failed on %lu, but bitmask: 0x%lx",
				     (unsigned long)i, malloc_fail_bitmask);
				++failures;
			}
		}
		rv_err = foo_b(foo);
		if (rv_err) {
			++err;
			if (!malloc_fail_bitmask) {
				logerror
				    ("foo_b failed on %lu, but bitmask: 0x%lx",
				     (unsigned long)i, malloc_fail_bitmask);
				++failures;
			}
		}
	}

	if (!err && (malloc_fail_bitmask && (malloc_fail_bitmask <= loops))) {
		logerror("no errors, but malloc_fail_bitmask is 0x%lx",
			 malloc_fail_bitmask);
		++failures;
	}
	if (err && !malloc_fail_bitmask) {
		++failures;
	}

end_test_out_of_memory:
	foo_free(foo);

	if (mctx.frees != mctx.allocs) {
		logerror("frees != allocs (%lu != %lu)", mctx.frees,
			 mctx.allocs);
		++failures;
	}
	if (mctx.free_bytes != mctx.alloc_bytes) {
		logerror("bytes freed != bytes alloc'd (%lu != %lu)",
			 mctx.free_bytes, mctx.alloc_bytes);
		++failures;
	}
	failures += (mctx.free_bytes == mctx.alloc_bytes ? 0 : 1);

	return failures;
}

int main(void)
{
	int i, failures = 0;

	failures += test_out_of_memory_loop(0);
	for (i = 0; i < 16; ++i) {
		failures += test_out_of_memory_loop(1UL << i);
	}

	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
