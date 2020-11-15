/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* oom-injecting-malloc.h */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#ifndef OOM_INJECTING_MALLOC_H
#define OOM_INJECTING_MALLOC_H 1

#ifdef __cplusplus
#define Oom_injecting_begin_C_functions extern "C" {
#define Oom_injecting_end_C_functions }
#else
#define Oom_injecting_begin_C_functions
#define Oom_injecting_end_C_functions
#endif

Oom_injecting_begin_C_functions
#undef Oom_injecting_begin_C_functions
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

void oom_injecting_context_init(oom_injecting_context_s *context);

void *oom_injecting_malloc(void *context, size_t size);

void oom_injecting_free(void *context, void *ptr);

/* function pointers to stdlib functions, set to non-NULL if freestanding */
extern void *(*oom_injecting_memset)(void *s, int c, size_t n);
extern void *(*oom_injecting_memcpy)(void *dest, const void *src, size_t n);

extern void *oom_injecting_errlog_context;
extern void (*oom_injecting_errlog_s)(void *context, const char *str);
extern void (*oom_injecting_errlog_z)(void *context, size_t z);
extern void (*oom_injecting_errlog_eol)(void *context);

Oom_injecting_end_C_functions
#undef Oom_injecting_end_C_functions
#endif /* OOM_INJECTING_MALLOC_H */
