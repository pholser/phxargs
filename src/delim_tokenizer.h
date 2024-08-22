#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

#include "buffer.h"
#include "options.h"

typedef struct {
  buffer* buf;

  uint8_t terminate_on_too_large_token;
  char delim;
} delim_tokenizer;

void init_delim_tokenizer(
  delim_tokenizer* const t,
  const options* const opts);

char* next_delim_token(delim_tokenizer* const t);

void free_delim_tokenizer(const delim_tokenizer* const t);

#endif  // PHXARGS_DELIM_TOKENIZER_H
