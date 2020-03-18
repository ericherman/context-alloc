/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* oom-injecting-malloc.h */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#ifndef OOM_INJECTING_MALLOC_H
#define OOM_INJECTING_MALLOC_H 1

#include <stddef.h>

typedef struct oom_injecting_context {
	unsigned long allocs;
	unsigned long alloc_bytes;
	unsigned long frees;
	unsigned long free_bytes;
	unsigned long fails;
	unsigned long max_used;
	unsigned long attempts;
	unsigned long attempts_to_fail_bitmask;
} oom_injecting_context_s;

void *oom_injecting_malloc(void *context, size_t size);

void oom_injecting_free(void *context, void *ptr);

#endif /* OOM_INJECTING_MALLOC_H */
