#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "tokenizer.h"

void tokenizer_init(
  tokenizer* t,
  tokenizer_ops* ops,
  size_t buffer_size) {

  t->ops = ops;
  buffer_init(&(t->buf), buffer_size);
}

char* tokenizer_next_token(
  tokenizer* t,
  FILE* arg_source,
  command* cmd) {

  return t->ops->next_token(t, arg_source, cmd);
}

size_t tokenizer_pos(tokenizer* t) {
  return buffer_pos(&(t->buf));
}

void tokenizer_add(tokenizer* t, char ch) {
  buffer_put(&(t->buf), ch);
}

char* tokenizer_token(tokenizer* t, size_t pos) {
  return t->buf.buf + pos;
}

void tokenizer_destroy(tokenizer* t) {
  buffer_destroy(&(t->buf));
  free(t);
}
