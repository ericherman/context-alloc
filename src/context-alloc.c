/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* context-alloc.c - malloc/free wrappers for a pluggable allocator */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

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

#if CONTEXT_ALLOC_HOSTED
#include <stdlib.h>
void *(*stdlib_malloc)(size_t size) = malloc;
void *(*stdlib_realloc)(void *ptr, size_t size) = realloc;
void *(*stdlib_calloc)(size_t nmemb, size_t size) = calloc;
void (*stdlib_free)(void *ptr) = free;
#else
void *(*stdlib_malloc)(size_t size);
void *(*stdlib_realloc)(void *ptr, size_t size);
void *(*stdlib_calloc)(size_t nmemb, size_t size);
void (*stdlib_free)(void *ptr);
#endif

void *context_stdlib_malloc(void *context, size_t size)
{
	(void)context;
	return stdlib_malloc(size);
}

void *context_stdlib_realloc(void *context, void *ptr, size_t size)
{
	(void)context;
	return stdlib_realloc(ptr, size);
}

void *context_stdlib_calloc(void *context, size_t nmemb, size_t size)
{
	(void)context;
	return stdlib_calloc(nmemb, size);
}

void *context_stdlib_reallocarray(void *context, void *ptr, size_t nmemb,
				  size_t size)
{
	(void)context;
	return stdlib_realloc(ptr, nmemb * size);
}

void context_stdlib_free(void *context, void *ptr)
{
	(void)context;
	stdlib_free(ptr);
}
