#ifndef PHXARGS_SPACE_TOKENIZER_H
#define PHXARGS_SPACE_TOKENIZER_H

typedef struct _space_tokenizer space_tokenizer;

space_tokenizer* space_tokenizer_create(
  size_t buffer_size,
  uint8_t line_mode,
  char* logical_end_of_input_marker);

void space_tokenizer_destroy(space_tokenizer* t);

#endif  // PHXARGS_SPACE_TOKENIZER_H
