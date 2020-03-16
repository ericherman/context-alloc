#include "foo.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

struct foo_list_s {
	char *data;
	struct foo_list_s *next;
};

struct foo_s {
	struct foo_list_s *list;
	int num;

	context_malloc_func alloc;
	context_free_func free;
	void *mem_context;
};

/* methods */
int foo_a(struct foo_s *foo)
{
	assert(foo);

	char buf[64];
	snprintf(buf, 64, "%d", foo->num);

	size_t size = sizeof(struct foo_list_s);
	struct foo_list_s *node = foo->alloc(foo->mem_context, size);
	if (!node) {
		return 1;
	}

	size = 1 + strlen(buf);
	node->data = foo->alloc(foo->mem_context, size);
	if (!node->data) {
		foo->free(foo->mem_context, node);
		return 1;
	}
	node->next = foo->list;
	foo->list = node;
	foo->num++;

	return 0;
}

int foo_b(struct foo_s *foo)
{
	assert(foo);

	struct foo_list_s *node = foo->list;
	if (node) {
		foo->list = node->next;
		foo->free(foo->mem_context, node->data);
		foo->free(foo->mem_context, node);
	}
	foo->num--;
	return 0;

}

/* constructors */
struct foo_s *foo_new(void)
{
	return foo_new_custom_allocator(NULL, NULL, NULL);
}

struct foo_s *foo_new_custom_allocator(context_malloc_func ealloc,
				       context_free_func efree, void *mem_context)
{
	assert((ealloc != NULL && efree != NULL)
	       || (ealloc == NULL && efree == NULL));
	if (!ealloc || !efree) {
		ealloc = context_stdlib_malloc;
		efree = context_stdlib_free;
		mem_context = NULL;
	}

	size_t size = sizeof(struct foo_s);
	struct foo_s *foo = ealloc(mem_context, size);
	if (!foo) {
		return NULL;
	}

	foo->list = NULL;
	foo->num = 0;

	foo->alloc = ealloc;
	foo->free = efree;
	foo->mem_context = mem_context;

	return foo;
}

/* destructors */
void foo_free(struct foo_s *foo)
{
	if (!foo) {
		return;
	}

	context_free_func efree = foo->free;
	void *mem_context = foo->mem_context;

	while (foo->list != NULL) {
		struct foo_list_s *node = foo->list;
		foo->list = node->next;
		efree(mem_context, node);
	}

	efree(mem_context, foo);
}
