#ifndef PHXARGS_COMMAND_ARGS_H
#define PHXARGS_COMMAND_ARGS_H

#include <stddef.h>

typedef struct {
  size_t count;
  size_t capacity;
  char** args;
  size_t length;
} command_args;

void init_args(command_args* const args);

void init_args_with_capacity(command_args* const args, size_t capacity);

void add_arg(command_args* const args, const char* const new_arg);

command_args* clone_args(const command_args* const args);

void free_args(const command_args* const args);

#endif  // PHXARGS_COMMAND_ARGS_H
