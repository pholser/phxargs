#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

#include <stddef.h>

#include "tokenizer.h"

typedef struct _delim_tokenizer delim_tokenizer;

delim_tokenizer* delim_tokenizer_create(
  size_t buffer_size,
  char arg_delimiter);

tokenizer* delim_tokenizer_as_tokenizer(delim_tokenizer* t);

void delim_tokenizer_destroy(delim_tokenizer* t);

#endif  // PHXARGS_DELIM_TOKENIZER_H
