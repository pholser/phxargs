#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include <stddef.h>
#include <stdio.h>

#include "command.h"
#include "options.h"

typedef enum {
  SPACE,
  DELIMITED
} tokenizer_kind;

typedef struct {
  tokenizer_kind t_kind;
  void* tokenizer;
} tokenizer;

void init_tokenizer(
  tokenizer* t,
  const options* const opts,
  size_t buffer_size);

char* next_token(tokenizer* const t, FILE* arg_source, command* cmd);

void free_tokenizer(const tokenizer* const t);

#endif  // PHXARGS_TOKENIZER_H
