#include <stdlib.h>

#include "buffer.h"
#include "tokenizer.h"
#include "util.h"

struct _tokenizer {
  tokenizer_ops* ops;
  buffer* buf;
  void* impl;
  tokenizer_error err;
};

tokenizer* tokenizer_create(
  tokenizer_ops* ops,
  size_t buffer_size,
  void* impl) {

  tokenizer* t = safe_malloc(sizeof(tokenizer));

  t->ops = ops;
  t->buf = buffer_create(buffer_size);
  t->impl = impl;
  t->err = TOKENIZER_ERR_NONE;

  return t;
}

void* tokenizer_impl(const tokenizer* t) {
  return t->impl;
}

char* tokenizer_next_token(tokenizer* t, FILE* arg_source) {
  return t->ops->next_token(t, arg_source);
}

size_t tokenizer_pos(const tokenizer* t) {
  return buffer_pos(t->buf);
}

void tokenizer_add(tokenizer* t, char ch) {
  buffer_put(t->buf, ch);
}

char* tokenizer_token(const tokenizer* t, size_t pos) {
  return buffer_start(t->buf) + pos;
}

void tokenizer_reset(tokenizer* t) {
  buffer_reset(t->buf);
}

void tokenizer_set_error(tokenizer* t, tokenizer_error err) {
  t->err = err;
}

tokenizer_error tokenizer_get_error(const tokenizer* t) {
  return t->err;
}

void tokenizer_destroy(tokenizer* t) {
  t->ops->destroy_impl(t->impl);
  buffer_destroy(t->buf);
  free(t);
}
