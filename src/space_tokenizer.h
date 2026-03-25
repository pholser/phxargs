#ifndef PHXARGS_SPACE_TOKENIZER_H
#define PHXARGS_SPACE_TOKENIZER_H

#include <stddef.h>
#include <stdint.h>

#include "tokenizer.h"

typedef struct _space_tokenizer space_tokenizer;

space_tokenizer* space_tokenizer_create(
  size_t buffer_size,
  char* logical_end_of_input_marker);

tokenizer* space_tokenizer_base(space_tokenizer* t);

void space_tokenizer_destroy(space_tokenizer* t);

#endif  // PHXARGS_SPACE_TOKENIZER_H
