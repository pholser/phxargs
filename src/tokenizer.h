#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include <stdbool.h>

#include "buffer.h"
#include "options.h"

typedef enum {
  NO_TOKEN,
  IN_TOKEN,
  IN_QUOTED_TOKEN,
  IN_TOKEN_ESCAPE
} tokenizer_state;

typedef struct {
  buffer* buf;
  tokenizer_state state;
  int quote_char;
  size_t token_start;
} tokenizer;

void init_tokenizer(tokenizer* t, const options* opts);

char* next_token(tokenizer* t);

void free_tokenizer(tokenizer* t);

#endif  // PHXARGS_TOKENIZER_H
