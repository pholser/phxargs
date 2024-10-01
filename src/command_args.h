#ifndef PHXARGS_COMMAND_ARGS_H
#define PHXARGS_COMMAND_ARGS_H

#include <stddef.h>

typedef struct {
  size_t count;
  size_t capacity;
  char** args;
  size_t length;
} command_args;

void init_args(command_args* args);

void init_args_with_capacity(command_args* args, size_t capacity);

void add_arg(command_args* args, char* new_arg);

command_args* clone_args(command_args* args);

void free_args(command_args* args);

#endif  // PHXARGS_COMMAND_ARGS_H
