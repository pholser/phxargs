#ifndef PHXARGS_EXECUTION_CONTEXT_H
#define PHXARGS_EXECUTION_CONTEXT_H

#include "command.h"

typedef enum {
  SPACE,
  DELIMITED
} tokenizer_kind;

typedef struct {
  command cmd;
  tokenizer_kind t_kind;
  void* tokenizer;
} execution_context;

void establish_context(execution_context* const ctx, int argc, char** argv);

int run_xargs(execution_context* const ctx);

void release_context(const execution_context* const ctx);

#endif  // PHXARGS_EXECUTION_CONTEXT_H
