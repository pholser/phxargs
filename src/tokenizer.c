#include <stddef.h>
#include <stdio.h>

#include "delim_tokenizer.h"
#include "options.h"
#include "space_tokenizer.h"
#include "tokenizer.h"
#include "util.h"

void init_tokenizer(
  tokenizer* const t,
  const options* const opts,
  size_t buffer_size) {

  if (opts->use_nul_char_as_arg_delimiter || opts->arg_delimiter != '\0') {
    t->t_kind = DELIMITED;
    t->tokenizer = safe_malloc(sizeof(delim_tokenizer));
    init_delim_tokenizer(
      t->tokenizer,
      buffer_size,
      opts->arg_delimiter);
  } else {
    t->t_kind = SPACE;
    t->tokenizer = safe_malloc(sizeof(space_tokenizer));
    init_space_tokenizer(
      t->tokenizer,
      buffer_size,
      options_line_mode(opts),
      opts->logical_end_of_input_marker);
  }
}

char* next_token(tokenizer* const t, FILE* arg_source, command* const cmd) {
  switch (t->t_kind) {
    case DELIMITED:
      return next_delim_token(
        (delim_tokenizer*) t->tokenizer,
        arg_source,
        cmd);
    case SPACE:
      return next_space_token(
        (space_tokenizer*) t->tokenizer,
        arg_source,
        cmd);
  }
}

void free_tokenizer(const tokenizer* const t) {
  switch (t->t_kind) {
    case DELIMITED:
      free_delim_tokenizer((delim_tokenizer*) t->tokenizer);
      break;
    case SPACE:
      free_space_tokenizer((space_tokenizer*) t->tokenizer);
      break;
  }
}
