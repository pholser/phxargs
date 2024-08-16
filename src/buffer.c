#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "util.h"

void init_buffer(buffer* buf, size_t sz) {
  buf->buf = safe_malloc(sz);
  memset(buf->buf, 0, sz);
  buf->sz = 0;
  buf->pos = 0;
}

size_t buffer_pos(buffer* buf) {
  return buf->pos;
}

void buffer_put(buffer* buf, char ch) {
  buf->buf[buf->pos++] = ch;
}

uint8_t buffer_full(buffer* buf) {
  return buf->pos == buf->sz;
}

void free_buffer(buffer* buf) {
  free(buf->buf);
}
