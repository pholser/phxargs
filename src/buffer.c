#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "util.h"

void init_buffer(buffer* const buf, size_t sz) {
  buf->buf = safe_calloc(sz + 1, sizeof(char));
  buf->sz = sz;
  buf->pos = 0;
}

size_t buffer_pos(const buffer* const buf) {
  return buf->pos;
}

void buffer_put(buffer* const buf, char ch) {
  buf->buf[buf->pos++] = ch;
}

uint8_t buffer_full(const buffer* const buf) {
  return buf->pos == buf->sz;
}

void free_buffer(const buffer* const buf) {
  free(buf->buf);
}
