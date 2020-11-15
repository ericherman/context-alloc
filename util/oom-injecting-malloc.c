/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* oom-injecting-malloc.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "oom-injecting-malloc.h"
#include "context-alloc.h"

#ifdef ARDUINO
#define CONTEXT_ALLOC_HOSTED 0
#endif

#ifndef CONTEXT_ALLOC_HOSTED
#ifdef __STDC_HOSTED__
#define CONTEXT_ALLOC_HOSTED __STDC_HOSTED__
#else
#define CONTEXT_ALLOC_HOSTED 1
#endif
#endif

void *oom_injecting_errlog_context = NULL;

#if CONTEXT_ALLOC_HOSTED
#include <string.h>
void *(*oom_injecting_memset)(void *s, int c, size_t n) = memset;
void *(*oom_injecting_memcpy)(void *dest, const void *src, size_t n) = memcpy;

#include <stdio.h>
FILE *oom_injecting_context_log(void *context)
{
	FILE *err = NULL;
	if (oom_injecting_errlog_context) {
		err = oom_injecting_errlog_context;
	} else {
		err = stderr;
	}

	/* avoid POSIX undefined stdout+stderr */
	/* https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */
	if (err == stderr) {
		fflush(stdout);
	}

	return err;
}

void oom_injecting_fprintf_s(void *context, const char *str)
{
	fprintf(oom_injecting_context_log(context), "%s", str);
}

void (*oom_injecting_errlog_s)(void *context, const char *str) =
    oom_injecting_fprintf_s;

void oom_injecting_fprintf_z(void *context, size_t z)
{
	fprintf(oom_injecting_context_log(context), "%lu", (unsigned long)z);
}

void (*oom_injecting_errlog_z)(void *context, size_t z) =
    oom_injecting_fprintf_z;

void oom_injecting_fprintf_eol(void *context)
{
	fprintf(oom_injecting_context_log(context), "\n");
}

void (*oom_injecting_errlog_eol)(void *context) = oom_injecting_fprintf_eol;

#else
void *(*oom_injecting_memset)(void *s, int c, size_t n) = NULL;
void *(*oom_injecting_memcpy)(void *dest, const void *src, size_t n) = NULL;

void (*oom_injecting_errlog_s)(void *context, const char *str) = NULL;
void (*oom_injecting_errlog_z)(void *context, size_t z) = NULL;
void (*oom_injecting_errlog_eol)(void *context) = NULL;

#endif

void oom_injecting_context_init(oom_injecting_context_s *context)
{
	oom_injecting_memset(context, 0x00, sizeof(oom_injecting_context_s));
}

void *oom_injecting_malloc(void *context, size_t size)
{
	void *log = oom_injecting_errlog_context;
	oom_injecting_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	void *ptr = NULL;
	size_t used = 0;

	ptr = NULL;
	ctx = (oom_injecting_context_s *)context;
	if (0x01 & (ctx->attempts_to_fail_bitmask >> ctx->attempts++)) {
		return NULL;
	}
	tracking_buffer = stdlib_malloc(sizeof(size_t) + size);
	if (!tracking_buffer) {
		++ctx->fails;
		return NULL;
	}

	oom_injecting_memcpy(tracking_buffer, &size, sizeof(size_t));
	++ctx->allocs;
	ctx->alloc_bytes += size;
	if (ctx->free_bytes > ctx->alloc_bytes) {
		oom_injecting_errlog_s(log, __FILE__);
		oom_injecting_errlog_s(log, ":");
		oom_injecting_errlog_z(log, __LINE__);
		oom_injecting_errlog_s(log, " BAD MOJO: ");
		oom_injecting_errlog_s(log, " free_bytes > alloc_bytes?! (");
		oom_injecting_errlog_z(log, ctx->free_bytes);
		oom_injecting_errlog_s(log, " > ");
		oom_injecting_errlog_z(log, ctx->alloc_bytes);
		oom_injecting_errlog_s(log, ")");
		oom_injecting_errlog_eol(log);
	}
	used = ctx->alloc_bytes - ctx->free_bytes;
	if (used > ctx->max_used) {
		ctx->max_used = used;
	}
	ptr = (void *)(tracking_buffer + sizeof(size_t));
	return ptr;
}

void oom_injecting_free(void *context, void *ptr)
{
	void *log = oom_injecting_errlog_context;
	oom_injecting_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	size_t size = 0;

	ctx = (oom_injecting_context_s *)context;
	if (ptr == NULL) {
		++ctx->fails;
		return;
	}
	tracking_buffer = ((unsigned char *)ptr) - sizeof(size_t);
	oom_injecting_memcpy(&size, tracking_buffer, sizeof(size_t));
	ctx->free_bytes += size;
	++ctx->frees;
	stdlib_free(tracking_buffer);
	if (ctx->free_bytes > ctx->alloc_bytes) {
		oom_injecting_errlog_s(log, __FILE__);
		oom_injecting_errlog_s(log, ":");
		oom_injecting_errlog_z(log, __LINE__);
		oom_injecting_errlog_s(log, " BAD MOJO: ");
		oom_injecting_errlog_s(log, " free_bytes > alloc_bytes?! (");
		oom_injecting_errlog_z(log, ctx->free_bytes);
		oom_injecting_errlog_s(log, " > ");
		oom_injecting_errlog_z(log, ctx->alloc_bytes);
		oom_injecting_errlog_s(log, ")");
		oom_injecting_errlog_eol(log);
	}
}
