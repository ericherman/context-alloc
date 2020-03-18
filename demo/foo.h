#ifndef FOO_H
#define FOO_H 1

#include <context-alloc.h>

struct foo;
typedef struct foo foo_s;

/* methods */
int foo_a(foo_s *foo);
int foo_b(foo_s *foo);

/* constructors */
foo_s *foo_new(void);
foo_s *foo_new_custom_allocator(context_malloc_func ealloc,
				context_free_func efree, void *mem_context);

/* destructors */
void foo_free(foo_s *foo);

#endif /* FOO_H */
