#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

#include <stddef.h>

#include "tokenizer.h"

typedef struct delim_tokenizer_s delim_tokenizer;

delim_tokenizer* delim_tokenizer_create(
  size_t buffer_size,
  char arg_delimiter,
  input_boundary_fn on_input_boundary,
  void* on_input_boundary_ctx);

tokenizer* delim_tokenizer_as_tokenizer(const delim_tokenizer* t);

void delim_tokenizer_destroy(delim_tokenizer* t);

#endif // PHXARGS_DELIM_TOKENIZER_H
