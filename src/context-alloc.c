/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* context-alloc.c - malloc/free wrappers for a pluggable allocator */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "context-alloc.h"

#if __STDC_HOSTED__

#include <assert.h>
#include <stdlib.h>

void *context_stdlib_malloc(void *context, size_t size)
{
	assert(context == NULL);
	return malloc(size);
}

void *context_stdlib_realloc(void *context, void *ptr, size_t size)
{
	assert(context == NULL);
	return realloc(ptr, size);
}

void *context_stdlib_calloc(void *context, size_t nmemb, size_t size)
{
	assert(context == NULL);
	return calloc(nmemb, size);
}

void *context_stdlib_reallocarray(void *context, void *ptr, size_t nmemb,
			       size_t size)
{
	assert(context == NULL);
#if _GNU_SOURCE
	return reallocarray(ptr, nmemb, size);
#else
	return realloc(ptr, nmemb * size);
#endif
}

void context_stdlib_free(void *context, void *ptr)
{
	assert(context == NULL);
	free(ptr);
}

#endif /* __STDC_HOSTED__ */
