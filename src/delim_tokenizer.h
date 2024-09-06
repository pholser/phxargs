#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

#include <stdio.h>

#include "buffer.h"

typedef struct {
  buffer* buf;
  size_t token_start;

  char delim;
} delim_tokenizer;

void init_delim_tokenizer(
  delim_tokenizer* const t,
  size_t buffer_size,
  char arg_delimiter);

char* next_delim_token(
  delim_tokenizer* const t,
  FILE* token_src,
  command* const cmd);

void free_delim_tokenizer(const delim_tokenizer* const t);

#endif  // PHXARGS_DELIM_TOKENIZER_H
