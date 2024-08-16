#ifndef PHXARGS_EXECUTION_CONTEXT_H
#define PHXARGS_EXECUTION_CONTEXT_H

#include "command.h"
#include "tokenizer.h"

typedef struct {
  command cmd;
  tokenizer tokenizer;
} execution_context;

void establish_context(execution_context* ctx, int argc, char** argv);

int run_xargs(execution_context* ctx);

void release_context(execution_context* ctx);

#endif  // PHXARGS_EXECUTION_CONTEXT_H
