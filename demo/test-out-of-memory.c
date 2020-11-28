/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-out-of-memory.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "demo-object.h"
#include <oom-injecting-malloc.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	context_malloc_func ctx_alloc;
	context_free_func ctx_free;
	oom_injecting_context_s mctx;
	demo_object_s *demo_object;
	int rv_err;

	ctx_alloc = oom_injecting_malloc;
	ctx_free = oom_injecting_free;
	oom_injecting_context_init(&mctx);

	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	demo_object =
	    demo_object_new_custom_allocator(ctx_alloc, ctx_free, &mctx);
	if (!demo_object) {
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
		rv_err = demo_object_a(demo_object);
		if (rv_err) {
			++err;
			if (!malloc_fail_bitmask) {
				logerror
				    ("demo_object_a failed on %lu, but bitmask: 0x%lx",
				     (unsigned long)i, malloc_fail_bitmask);
				++failures;
			}
		}
		rv_err = demo_object_b(demo_object);
		if (rv_err) {
			++err;
			if (!malloc_fail_bitmask) {
				logerror
				    ("demo_object_b failed on %lu, but bitmask: 0x%lx",
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
	demo_object_free(demo_object);

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

int test_destroy_cleanup(void)
{
	int failures = 0;
	context_malloc_func ctx_alloc;
	context_free_func ctx_free;
	oom_injecting_context_s mctx;
	demo_object_s *demo_object;

	ctx_alloc = oom_injecting_malloc;
	ctx_free = oom_injecting_free;
	oom_injecting_context_init(&mctx);

	demo_object =
	    demo_object_new_custom_allocator(ctx_alloc, ctx_free, &mctx);

	failures += demo_object_a(demo_object);
	failures += demo_object_a(demo_object);
	failures += demo_object_a(demo_object);

	failures += demo_object_b(demo_object);

	demo_object_free(demo_object);

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

int test_default_constuctor_destructor(void)
{
	int failures = 0;
	demo_object_s *demo_object = NULL;
	demo_object = demo_object_new();
	if (!demo_object) {
		return 1;
	}

	demo_object_free(demo_object);

	return failures;
}

int main(void)
{
	int i, failures = 0;

	failures += test_default_constuctor_destructor();

	failures += test_destroy_cleanup();

	failures += test_out_of_memory_loop(0);
	for (i = 0; i < 16; ++i) {
		failures += test_out_of_memory_loop(1UL << i);
	}

	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
