#ifndef PHXARGS_DELIM_TOKENIZER_H
#define PHXARGS_DELIM_TOKENIZER_H

typedef struct _delim_tokenizer delim_tokenizer;

delim_tokenizer* delim_tokenizer_create(
  size_t buffer_size,
  char arg_delimiter);

void delim_tokenizer_destroy(delim_tokenizer* t);

#endif  // PHXARGS_DELIM_TOKENIZER_H
