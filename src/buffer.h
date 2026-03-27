#ifndef PHXARGS_BUFFER_H
#define PHXARGS_BUFFER_H

#include <stddef.h>

typedef struct buffer_s buffer;

buffer* buffer_create(size_t sz);

char* buffer_start(const buffer* buf);

size_t buffer_pos(const buffer* buf);

void buffer_put(buffer* buf, char ch);

void buffer_reset(buffer* buf);

void buffer_destroy(buffer* buf);

#endif  // PHXARGS_BUFFER_H
