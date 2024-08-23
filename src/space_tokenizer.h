#ifndef PHXARGS_SPACE_TOKENIZER_H
#define PHXARGS_SPACE_TOKENIZER_H

#include "buffer.h"
#include "command.h"
#include "options.h"

typedef enum {
  NO_TOKEN,
  NO_TOKEN_ESCAPE,
  IN_TOKEN,
  IN_QUOTED_TOKEN,
  IN_TOKEN_ESCAPE
} space_tokenizer_state;

typedef struct {
  buffer* buf;

  space_tokenizer_state state;
  int quote_char;
  size_t token_start;

  uint8_t line_mode;
  char* logical_end_of_input_marker;
} space_tokenizer;

void init_space_tokenizer(
  space_tokenizer* const t,
  const options* const opts);

char* next_space_token(space_tokenizer* const t, command* const cmd);

void free_space_tokenizer(const space_tokenizer* const t);

#endif  // PHXARGS_SPACE_TOKENIZER_H
