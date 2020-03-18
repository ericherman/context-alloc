#ifndef FOO_H
#define FOO_H 1

#include <context-alloc.h>

struct foo_s;

/* methods */
int foo_a(struct foo_s *foo);
int foo_b(struct foo_s *foo);

/* constructors */
struct foo_s *foo_new(void);
struct foo_s *foo_new_custom_allocator(context_malloc_func ealloc,
				       context_free_func efree,
				       void *mem_context);

/* destructors */
void foo_free(struct foo_s *foo);

#endif /* FOO_H */
