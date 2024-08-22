#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "delim_tokenizer.h"
#include "options.h"
#include "util.h"

void init_delim_tokenizer(
  delim_tokenizer* const t,
  const options* const opts) {

  t->buf = safe_malloc(sizeof(buffer));
  init_buffer(t->buf, opts->max_command_length);

  t->terminate_on_too_large_token = opts->terminate_on_too_large_command;
  t->delim = opts->arg_delimiter;

  t->token_start = 0;
}

void delim_tokenizer_start_token(delim_tokenizer* const t) {
  t->token_start = buffer_pos(t->buf);
}

void delim_tokenizer_append_to_token(const delim_tokenizer* const t, int ch) {
  if (t->terminate_on_too_large_token && buffer_full(t->buf)) {
    fprintf(stderr, "phxargs: insufficient space for argument\n");
    exit(EXIT_FAILURE);
  }
  buffer_put(t->buf, (char) ch);
}

char* delim_tokenizer_end_token(delim_tokenizer* const t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;

  return token;
}

char* next_delim_token(delim_tokenizer* const t) {
  delim_tokenizer_start_token(t);

  int ch;
  while ((ch = getc(stdin)) != EOF) {
    if (ch != t->delim) {
      delim_tokenizer_append_to_token(t, ch);
    } else {
      return delim_tokenizer_end_token(t);
    }
  }

  if (ferror(stdin)) {
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
}
