#include "alloc.h"

#include <stdio.h>
#include <stdlib.h>

// Globals

Joy_Alloc *alloc_heap;

// Functions

static Joy_Alloc *get_tag(const void *ptr)
{
	return (Joy_Alloc *)ptr - 1;
}

static bool is_tag_marked(const Joy_Alloc *tag)
{
	return tag->prev & 1;
}

static void mark_tag(Joy_Alloc *tag)
{
	tag->prev |= 1;
}

static Joy_Alloc *get_tag_prev(Joy_Alloc *tag)
{
	return (Joy_Alloc *)(tag->prev >> 1);
}

static void set_tag_prev(Joy_Alloc *tag, Joy_Alloc *prev)
{
	tag->prev = (uintptr_t)prev << 1;
}

static Joy_Alloc *pop_tag(Joy_Alloc *tag)
{
	Joy_Alloc *prev = get_tag_prev(tag);
	free(tag);

	return prev;
}

void *new_alloc(size_t size)
{
	Joy_Alloc *tag = malloc(sizeof(Joy_Alloc) + size);
	if (!tag) {
		perror("Fatal Error");
		exit(1);
	}
	set_tag_prev(tag, alloc_heap);

	return tag;
}

bool alloc_mark(void *ptr)
{
	if (!ptr) {
		return true;
	}

	Joy_Alloc *tag = get_tag(ptr);
	if (is_tag_marked(tag)) {
		return true;
	}
	mark_tag(tag);

	return false;
}

void alloc_collect(void)
{
	while (alloc_heap && !is_tag_marked(alloc_heap)) {
		alloc_heap = pop_tag(alloc_heap);
	}

	for (Joy_Alloc *tag = alloc_heap; tag; tag = get_tag_prev(tag)) {
		while (get_tag_prev(tag) && !is_tag_marked(get_tag_prev(tag))) {
			set_tag_prev(tag, pop_tag(get_tag_prev(tag)));
		}
	}
}
