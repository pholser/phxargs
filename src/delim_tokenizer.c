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
}

char* next_delim_token(delim_tokenizer* const t) {
  int ch;
  while ((ch = getc(stdin)) != EOF) {
    if (t->terminate_on_too_large_token && buffer_full(t->buf)) {
      fprintf(stderr, "phxargs: insufficient space for argument\n");
      exit(EXIT_FAILURE);
    }

    if (ch == t->delim) {
      buffer_put(t->buf, '\0');
      return t->buf->buf;
    }
  }

  if (ferror(stdin)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else {
    buffer_put(t->buf, '\0');
    char* token = t->buf->buf;
    return token;
  }
}

void free_delim_tokenizer(const delim_tokenizer* const t) {
  free_buffer(t->buf);
}
