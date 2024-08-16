#ifndef PHXARGS_BUFFER_H
#define PHXARGS_BUFFER_H

#include <stddef.h>

typedef struct {
  char* buf;
  size_t sz;
  size_t pos;
} buffer;

void init_buffer(buffer* buf, size_t sz);

size_t buffer_pos(buffer* buf);

void buffer_put(buffer* buf, char ch);

void free_buffer(buffer* buf);

#endif  // PHXARGS_BUFFER_H
