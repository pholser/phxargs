#ifndef PHXARGS_BUFFER_H
#define PHXARGS_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  char* buf;
  size_t sz;
  size_t pos;
} buffer;

void init_buffer(buffer* const buf, size_t sz);

size_t buffer_pos(const buffer* const buf);

void buffer_put(buffer* const buf, char ch);

uint8_t buffer_full(const buffer* const buf);

void free_buffer(const buffer* const buf);

#endif  // PHXARGS_BUFFER_H
