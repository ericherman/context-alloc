/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* oom-injecting-malloc.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "oom-injecting-malloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void oom_injecting_context_init(oom_injecting_context_s *context)
{
	memset(context, 0x00, sizeof(oom_injecting_context_s));
}

void *oom_injecting_malloc(void *context, size_t size)
{
	oom_injecting_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	void *ptr = NULL;
	size_t used = 0;

	ptr = NULL;
	ctx = (oom_injecting_context_s *)context;
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

void oom_injecting_free(void *context, void *ptr)
{
	oom_injecting_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	size_t size = 0;

	ctx = (oom_injecting_context_s *)context;
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
		fflush(stdout);	/* avoid POSIX undefined stdout+stderr */
		/* https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */
		fprintf(stderr,
			"%s: %d BAD MOJO: free_bytes > alloc_bytes?!"
			" (%lu > %lu) just freed %lu\n", __FILE__, __LINE__,
			(unsigned long)ctx->free_bytes,
			(unsigned long)ctx->alloc_bytes, (unsigned long)size);
	}

}
