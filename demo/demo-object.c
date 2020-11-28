#include "demo-object.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "malloc-whine.h"

struct demo_object_list;
typedef struct demo_object_list demo_object_list_s;

struct demo_object_list {
	char *data;
	demo_object_list_s *next;
};

struct demo_object {
	demo_object_list_s *list;
	int num;

	context_malloc_func alloc;
	context_free_func free;
	void *mem_context;
};

/* methods */
int demo_object_a(demo_object_s *demo_object)
{
	assert(demo_object);

	char buf[64];
	snprintf(buf, 64, "%d", demo_object->num);

	size_t size = sizeof(demo_object_list_s);
	demo_object_list_s *node =
	    (demo_object_list_s *)demo_object->alloc(demo_object->mem_context,
						     size);
	if (!node) {
		Malloc_whine(stdout, size);
		return 1;
	}

	size = 1 + strlen(buf);
	node->data = (char *)demo_object->alloc(demo_object->mem_context, size);
	if (!node->data) {
		Malloc_whine(stdout, size);
		demo_object->free(demo_object->mem_context, node);
		return 1;
	}
	node->next = demo_object->list;
	demo_object->list = node;
	demo_object->num++;

	return 0;
}

int demo_object_b(demo_object_s *demo_object)
{
	assert(demo_object);

	demo_object_list_s *node = demo_object->list;
	if (node) {
		demo_object->list = node->next;
		demo_object->free(demo_object->mem_context, node->data);
		demo_object->free(demo_object->mem_context, node);
	}
	demo_object->num--;
	return 0;

}

/* constructors */
demo_object_s *demo_object_new(void)
{
	return demo_object_new_custom_allocator(NULL, NULL, NULL);
}

demo_object_s *demo_object_new_custom_allocator(context_malloc_func c_alloc,
						context_free_func c_free,
						void *mem_context)
{
	assert((c_alloc != NULL && c_free != NULL)
	       || (c_alloc == NULL && c_free == NULL));
	if (!c_alloc || !c_free) {
		c_alloc = context_stdlib_malloc;
		c_free = context_stdlib_free;
		mem_context = NULL;
	}

	size_t size = sizeof(demo_object_s);
	demo_object_s *demo_object =
	    (demo_object_s *)c_alloc(mem_context, size);
	if (!demo_object) {
		Malloc_whine(stdout, size);
		return NULL;
	}

	demo_object->list = NULL;
	demo_object->num = 0;

	demo_object->alloc = c_alloc;
	demo_object->free = c_free;
	demo_object->mem_context = mem_context;

	return demo_object;
}

/* destructors */
void demo_object_free(demo_object_s *demo_object)
{
	if (!demo_object) {
		return;
	}

	context_free_func c_free = demo_object->free;
	void *mem_context = demo_object->mem_context;

	while (demo_object->list != NULL) {
		demo_object_list_s *node = demo_object->list;
		demo_object->list = node->next;
		demo_object->free(demo_object->mem_context, node->data);
		c_free(mem_context, node);
	}

	c_free(mem_context, demo_object);
}
