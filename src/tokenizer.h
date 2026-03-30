#ifndef PHXARGS_TOKENIZER_H
#define PHXARGS_TOKENIZER_H

#include <stdio.h>

typedef void (*line_count_fn)(void* ctx);

typedef enum {
  TOKENIZER_ERR_NONE = 0,
  TOKENIZER_ERR_IO,
  TOKENIZER_ERR_BACKSLASH_AT_EOF,
  TOKENIZER_ERR_UNTERMINATED_QUOTE
} tokenizer_error;

typedef struct tokenizer_s tokenizer;
typedef struct tokenizer_ops_s tokenizer_ops;

struct tokenizer_ops_s {
  char* (*next_token)(tokenizer* self, FILE* arg_source);
  void (*destroy_impl)(void* impl);
};

tokenizer* tokenizer_create(tokenizer_ops* ops, size_t buffer_size, void* impl);

void* tokenizer_impl(const tokenizer* t);

char* tokenizer_next_token(tokenizer* t, FILE* arg_source);

void tokenizer_set_error(tokenizer* t, tokenizer_error err);

tokenizer_error tokenizer_get_error(const tokenizer* t);

size_t tokenizer_pos(const tokenizer* t);

void tokenizer_add(tokenizer* t, char ch);

char* tokenizer_token(const tokenizer* t, size_t pos);

void tokenizer_reset(tokenizer* t);

void tokenizer_destroy(tokenizer* t);

#endif // PHXARGS_TOKENIZER_H
