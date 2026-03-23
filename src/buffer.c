#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "util.h"

struct _buffer {
  char* buf;
  size_t sz;
  size_t pos;
};

buffer* buffer_create(size_t sz) {
  buffer* buf = safe_malloc(sizeof(buffer));

  buf->buf = safe_calloc(sz + 1, sizeof(char));
  buf->sz = sz;
  buf->pos = 0;

  return buf;
}

char* buffer_start(buffer* buf) {
  return buf->buf;
}

size_t buffer_pos(buffer* buf) {
  return buf->pos;
}

void buffer_put(buffer* buf, char ch) {
  buf->buf[buf->pos++] = ch;
}

void buffer_reset(buffer* buf) {
  buf->pos = 0;
}

uint8_t buffer_full(buffer* buf) {
  return buf->pos == buf->sz;
}

void buffer_destroy(buffer* buf) {
  free(buf->buf);
  free(buf);
}
