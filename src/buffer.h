#ifndef PHXARGS_BUFFER_H
#define PHXARGS_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  char* buf;
  size_t sz;
  size_t pos;
} buffer;

void buffer_print(buffer* buf);

char* buffer_start(buffer* buf);

void buffer_init(buffer* buf, size_t sz);

size_t buffer_pos(buffer* buf);

void buffer_put(buffer* buf, char ch);

uint8_t buffer_full(buffer* buf);

void buffer_destroy(buffer* buf);

#endif  // PHXARGS_BUFFER_H
