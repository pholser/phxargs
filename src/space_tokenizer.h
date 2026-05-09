#ifndef PHXARGS_SPACE_TOKENIZER_H
#define PHXARGS_SPACE_TOKENIZER_H

#include <stddef.h>

#include "tokenizer.h"

typedef struct space_tokenizer_s space_tokenizer;

space_tokenizer* space_tokenizer_create(
  size_t buffer_size,
  const char* logical_end_of_input_marker,
  input_boundary_fn on_input_boundary,
  void* on_input_boundary_ctx);

tokenizer* space_tokenizer_as_tokenizer(const space_tokenizer* t);

void space_tokenizer_destroy(space_tokenizer* t);

#endif // PHXARGS_SPACE_TOKENIZER_H
