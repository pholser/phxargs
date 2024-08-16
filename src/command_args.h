#ifndef PHXARGS_COMMAND_ARGS_H
#define PHXARGS_COMMAND_ARGS_H

#include <stddef.h>

typedef struct {
  size_t count;
  size_t capacity;
  char** args;
} command_args;

command_args* allocate_args();

void add_arg(command_args* args, const char* new_arg);

void free_args(const command_args* args);

#endif  // PHXARGS_COMMAND_ARGS_H
