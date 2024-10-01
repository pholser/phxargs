#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "util.h"

void buffer_init(buffer* buf, size_t sz) {
  buf->buf = safe_calloc(sz + 1, sizeof(char));
  buf->sz = sz;
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

void buffer_destroy(buffer* buf) {
  free(buf->buf);
}
