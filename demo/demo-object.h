#ifndef DEMO_OBJECT_H
#define DEMO_OBJECT_H 1

#include <context-alloc.h>

struct demo_object;
typedef struct demo_object demo_object_s;

/* methods */
int demo_object_a(demo_object_s *demo_object);
int demo_object_b(demo_object_s *demo_object);

/* constructors */
demo_object_s *demo_object_new(void);
demo_object_s *demo_object_new_custom_allocator(context_malloc_func ealloc,
						context_free_func efree,
						void *mem_context);

/* destructors */
void demo_object_free(demo_object_s *demo_object);

#endif /* DEMO_OBJECT_H */
