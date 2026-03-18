#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "delim_tokenizer.h"
#include "tokenizer.h"
#include "util.h"

struct _delim_tokenizer {
  tokenizer* base;

  size_t token_start;
  char delim;
};

void delim_tokenizer_start_token(delim_tokenizer* t) {
  t->token_start = tokenizer_pos(t->base);
}

void delim_tokenizer_append_to_token(delim_tokenizer* t, int ch) {
  tokenizer_add(t->base, ch);
}

char* delim_tokenizer_end_token(delim_tokenizer* t) {
  tokenizer_add(t->base, '\0');
  return tokenizer_token(t->base, t->token_start);
}

char* next_delim_token(
  tokenizer* t,
  FILE* token_source,
  command* cmd) {

  delim_tokenizer* self = (delim_tokenizer*) tokenizer_impl(t);

  tokenizer_reset(t);
  delim_tokenizer_start_token(self);

  int ch;
  while ((ch = getc(token_source)) != EOF) {
    if (ch != self->delim) {
      delim_tokenizer_append_to_token(self, ch);
    } else {
      command_increment_line_count(cmd);
      return delim_tokenizer_end_token(self);
    }
  }

  if (ferror(token_source)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else if (self->token_start == tokenizer_pos(t)) {
    return NULL;
  } else {
    return delim_tokenizer_end_token(self);
  }
}

static void delim_tokenizer_destroy_impl(void* impl) {
  free(impl);
}

tokenizer_ops delim_tokenizer_ops = {
  .next_token = next_delim_token,
  .destroy_impl = delim_tokenizer_destroy_impl
};

delim_tokenizer* delim_tokenizer_create(
  size_t buffer_size,
  char arg_delimiter) {

  delim_tokenizer* t = safe_malloc(sizeof(delim_tokenizer));
  t->delim = arg_delimiter;
  t->token_start = 0;
  t->base = tokenizer_create(&delim_tokenizer_ops, buffer_size, t);

  return t;
}

tokenizer* delim_tokenizer_base(delim_tokenizer* t) {
  return t->base;
}

void delim_tokenizer_destroy(delim_tokenizer* t) {
  tokenizer_destroy(t->base);
}
