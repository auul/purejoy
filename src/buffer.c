#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Joy_Buffer *new_buffer(size_t cap)
{
	Joy_Buffer *buffer = malloc(sizeof(Joy_Buffer) + cap);
	if (!buffer) {
		perror("Fatal Error");
		exit(1);
	}

	buffer->cap = cap;
	buffer->at = 0;

	return buffer;
}

Joy_Buffer *buffer_destroy(Joy_Buffer *buffer)
{
	if (buffer) {
		free(buffer);
	}
	return NULL;
}

size_t buffer_get_len(const Joy_Buffer *buffer)
{
	return buffer->at;
}

size_t buffer_get_cap(const Joy_Buffer *buffer)
{
	return buffer->cap;
}

const char *buffer_get_raw(const Joy_Buffer *buffer)
{
	return buffer->raw;
}

static Joy_Buffer *expand_buffer(Joy_Buffer *buffer, size_t cap)
{
	buffer = realloc(buffer, sizeof(Joy_Buffer) + cap);
	if (!buffer) {
		perror("Fatal Error");
		exit(1);
	}
	buffer->cap = cap;

	return buffer;
}

Joy_Buffer *buffer_push(Joy_Buffer *buffer, const void *src, size_t size)
{
	if (buffer->at + size > buffer->cap) {
		buffer = expand_buffer(buffer, 2 * (buffer->at + size));
	}

	memcpy(buffer->raw + buffer->at, src, size);
	buffer->at += size;

	return buffer;
}
