#ifndef PHXARGS_BUFFER_H
#define PHXARGS_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct _buffer buffer;

buffer* buffer_create(size_t sz);

char* buffer_start(buffer* buf);

size_t buffer_pos(buffer* buf);

void buffer_put(buffer* buf, char ch);

void buffer_reset(buffer* buf);

uint8_t buffer_full(buffer* buf);

void buffer_destroy(buffer* buf);

#endif  // PHXARGS_BUFFER_H
