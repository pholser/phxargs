#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include "buffer.h"
#include "options.h"

typedef enum {
  NO_TOKEN,
  NO_TOKEN_ESCAPE,
  IN_TOKEN,
  IN_QUOTED_TOKEN,
  IN_TOKEN_ESCAPE
} tokenizer_state;

typedef struct {
  buffer* buf;

  tokenizer_state state;
  int quote_char;
  size_t token_start;

  uint8_t terminate_on_too_large_token;
  uint8_t line_mode;
} tokenizer;

void init_tokenizer(tokenizer* const t, const options* opts);

char* next_token(tokenizer* const t, command* const cmd);

void free_tokenizer(const tokenizer* const t);

#endif  // PHXARGS_TOKENIZER_H
