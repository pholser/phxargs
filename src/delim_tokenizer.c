#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "command.h"
#include "delim_tokenizer.h"
#include "util.h"

void init_delim_tokenizer(
  delim_tokenizer* const t,
  size_t buffer_size,
  char arg_delimiter) {

  t->buf = safe_malloc(sizeof(buffer));
  init_buffer(t->buf, buffer_size);

  t->delim = arg_delimiter;
  t->token_start = 0;
}

void delim_tokenizer_start_token(delim_tokenizer* const t) {
  t->token_start = buffer_pos(t->buf);
}

void delim_tokenizer_append_to_token(const delim_tokenizer* const t, int ch) {
  buffer_put(t->buf, (char) ch);
}

char* delim_tokenizer_end_token(delim_tokenizer* const t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;

  return token;
}

char* next_delim_token(
  delim_tokenizer* const t,
  FILE* token_source,
  command* const cmd) {

  delim_tokenizer_start_token(t);

  int ch;
  while ((ch = getc(token_source)) != EOF) {
    if (ch != t->delim) {
      delim_tokenizer_append_to_token(t, ch);
    } else {
      ++cmd->line_count;
      return delim_tokenizer_end_token(t);
    }
  }

  if (ferror(token_source)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else if (t->token_start == buffer_pos(t->buf)) {
    return NULL;
  } else {
    return delim_tokenizer_end_token(t);
  }
}

void free_delim_tokenizer(const delim_tokenizer* const t) {
  free_buffer(t->buf);
  free(t->buf);
}
