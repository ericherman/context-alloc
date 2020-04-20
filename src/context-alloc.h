/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* context-alloc.h - interface for a pluggable allocator */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#ifndef CONTEXT_ALLOC_H
#define CONTEXT_ALLOC_H 1

#include <stddef.h>		/* size_t */

typedef void *(*context_malloc_func)(void *context, size_t size);
typedef void *(*context_realloc_func)(void *context, void *ptr, size_t size);
typedef void *(*context_calloc_func)(void *context, size_t nmemb, size_t size);
typedef void *(*context_reallocarray_func)(void *context, void *ptr,
					   size_t nmemb, size_t size);
typedef void (*context_free_func)(void *context, void *ptr);

#if __STDC_HOSTED__
void *context_stdlib_malloc(void *context, size_t size);
void *context_stdlib_realloc(void *context, void *ptr, size_t size);
void *context_stdlib_calloc(void *context, size_t nmemb, size_t size);
void *context_stdlib_reallocarray(void *context, void *ptr, size_t nmemb,
				  size_t size);
void context_stdlib_free(void *context, void *ptr);
#endif /* __STDC_HOSTED__ */

#endif /* CONTEXT_ALLOC_H */
