#ifndef JOY_ALLOC_H
#define JOY_ALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Forward Declarations

typedef struct joy_alloc Joy_Alloc;

// Data Types

struct joy_alloc {
	uintptr_t prev;
	char ptr[];
};

// Functions

void *new_alloc(size_t size);
bool alloc_mark(void *ptr);
void alloc_collect(void);

#endif
