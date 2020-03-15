/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* easy-alloc.h - interface for a pluggable allocator */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#ifndef EASY_ALLOC_H
#define EASY_ALLOC_H 1

#include <stddef.h>		/* size_t */

typedef void *(*easy_malloc_func)(void *context, size_t size);
typedef void *(*easy_realloc_func)(void *context, void *ptr, size_t size);
typedef void *(*easy_calloc_func)(void *context, size_t nmemb, size_t size);
typedef void *(*easy_reallocarray_func)(void *context, void *ptr, size_t nmemb,
					size_t size);
typedef void (*easy_free_func)(void *context, void *ptr);

#if __STDC_HOSTED__
void *easy_stdlib_malloc(void *context, size_t size);
void *easy_stdlib_realloc(void *context, void *ptr, size_t size);
void *easy_stdlib_calloc(void *context, size_t nmemb, size_t size);
void *easy_stdlib_reallocarray(void *context, void *ptr, size_t nmemb,
			       size_t size);
void easy_stdlib_free(void *context, void *ptr);
#endif /* __STDC_HOSTED__ */

#endif /* EASY_ALLOC_H */
