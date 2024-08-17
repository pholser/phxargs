#ifndef PHXARGS_NULL_SEP_TOKENIZER_H
#define PHXARGS_NULL_SEP_TOKENIZER_H

#include "buffer.h"
#include "command.h"
#include "options.h"

typedef struct {
  buffer* buf;

  uint8_t terminate_on_too_large_token;
  uint8_t line_mode;
} null_sep_tokenizer;

void init_null_sep_tokenizer(
  null_sep_tokenizer* const t,
  const options* const opts);

char* next_null_sep_token(null_sep_tokenizer* const t, command* const cmd);

void free_null_sep_tokenizer(const null_sep_tokenizer* const t);

#endif  // PHXARGS_NULL_SEP_TOKENIZER_H
