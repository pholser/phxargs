#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

#include <stddef.h>

#include "tokenizer.h"

typedef struct delim_tokenizer_s delim_tokenizer;

delim_tokenizer* delim_tokenizer_create(
  size_t buffer_size,
  char arg_delimiter,
  line_count_fn on_line,
  void* on_line_ctx);

tokenizer* delim_tokenizer_as_tokenizer(delim_tokenizer* t);

void delim_tokenizer_destroy(delim_tokenizer* t);

#endif // PHXARGS_DELIM_TOKENIZER_H
