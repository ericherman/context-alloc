#ifndef FOO_H
#define FOO_H 1

#include <easy-alloc.h>

struct foo_s;

/* methods */
int foo_a(struct foo_s *foo);
int foo_b(struct foo_s *foo);

/* constructors */
struct foo_s *foo_new(void);
struct foo_s *foo_new_custom_allocator(easy_malloc_func ealloc,
				       easy_free_func efree, void *mem_context);

/* destructors */
void foo_free(struct foo_s *foo);

#endif /* FOO_H */
