#include "buffer.h"

#include <stddef.h>
#include <stdlib.h>

#include "util.h"

struct buffer_s {
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

char* buffer_start(const buffer* buf) {
  return buf->buf;
}

size_t buffer_pos(const buffer* buf) {
  return buf->pos;
}

void buffer_put(buffer* buf, char ch) {
  if (buf->pos == buf->sz) {
    buf->sz *= 2;
    buf->buf = safe_realloc(buf->buf, buf->sz + 1);
  }

  buf->buf[buf->pos++] = ch;
}

void buffer_reset(buffer* buf) {
  buf->pos = 0;
}

void buffer_destroy(buffer* buf) {
  free(buf->buf);
  free(buf);
}
