#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include <stdio.h>

#include "buffer.h"
#include "command.h"

typedef struct _tokenizer_ops tokenizer_ops;

typedef struct _tokenizer {
  tokenizer_ops* ops;
  buffer buf;
} tokenizer;

struct _tokenizer_ops {
  char* (*next_token)(
    tokenizer* self,
    FILE* arg_source,
    command* cmd);
};

void tokenizer_init(
  tokenizer* t,
  tokenizer_ops* ops,
  size_t buffer_size);

char* tokenizer_next_token(
  tokenizer* t,
  FILE* arg_source,
  command* cmd);

size_t tokenizer_pos(tokenizer* t);

void tokenizer_add(tokenizer* t, char ch);

char* tokenizer_token(tokenizer* t, size_t pos);

void tokenizer_destroy(tokenizer* t);

#endif  // PHXARGS_TOKENIZER_H
