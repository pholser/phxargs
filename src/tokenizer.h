#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include <stddef.h>
#include <stdio.h>

#include "command.h"

typedef struct _tokenizer tokenizer;
typedef struct _tokenizer_ops tokenizer_ops;

struct _tokenizer_ops {
  char* (*next_token)(
    tokenizer* self,
    FILE* arg_source,
    command* cmd);
  void (*destroy_impl)(void* impl);
};

tokenizer* tokenizer_create(
  tokenizer_ops* ops,
  size_t buffer_size,
  void* impl);

void* tokenizer_impl(tokenizer* t);

char* tokenizer_next_token(
  tokenizer* t,
  FILE* arg_source,
  command* cmd);

size_t tokenizer_pos(tokenizer* t);

void tokenizer_add(tokenizer* t, char ch);

char* tokenizer_token(tokenizer* t, size_t pos);

void tokenizer_reset(tokenizer* t);

void tokenizer_destroy(tokenizer* t);

#endif  // PHXARGS_TOKENIZER_H
