#ifndef PHXARGS_REPLACER_MODE_H
#define PHXARGS_REPLACER_MODE_H

#include <stdio.h>

#include "command.h"
#include "tokenizer.h"

typedef struct {
  command cmd;
  tokenizer toker;
  FILE* arg_source;
  char* placeholder;
} replacer_mode;

void init_replacer_mode(
  replacer_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv);

int run_replacer_mode(replacer_mode* const mode);

void free_replacer_mode(const replacer_mode* const mode);

#endif  // PHXARGS_REPLACER_MODE_H
