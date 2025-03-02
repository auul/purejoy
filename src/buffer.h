#ifndef JOY_BUFFER_H
#define JOY_BUFFER_H

#include <stddef.h>

// Forward Declarations

typedef struct joy_buffer Joy_Buffer;

// Data Types

struct joy_buffer {
	size_t cap;
	size_t at;
	char raw[];
};

// Constants

static const size_t JOY_BUFFER_INIT_CAP = 1;

// Functions

Joy_Buffer *new_buffer(size_t cap);
Joy_Buffer *buffer_destroy(Joy_Buffer *buffer);
size_t buffer_get_cap(const Joy_Buffer *buffer);
size_t buffer_get_len(const Joy_Buffer *buffer);
const char *buffer_get_raw(const Joy_Buffer *buffer);
Joy_Buffer *buffer_push(Joy_Buffer *buffer, const void *src, size_t size);

#endif
