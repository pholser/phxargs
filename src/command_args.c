#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command_args.h"
#include "util.h"

void init_args_with_capacity(command_args* args, size_t capacity) {
  args->count = 0;
  args->capacity = capacity;
  args->args = safe_calloc(args->capacity, sizeof(char*));
  args->length = 0;
}

void init_args(command_args* args) {
  init_args_with_capacity(args, 10);
}

void reallocate_args_if_needed(command_args* args) {
  if (args->count + 1 > args->capacity) {
    args->capacity *= 2;
    args->args =
      safe_realloc(args->args, args->capacity * sizeof(char*));
  }
}

void add_arg(command_args* args, char* new_arg) {
  reallocate_args_if_needed(args);

  args->args[args->count++] = safe_strdup(new_arg);
  args->length += strlen(new_arg) + 1;
}

command_args* clone_args(command_args* args) {
  command_args* copy = safe_malloc(sizeof(command_args));
  init_args_with_capacity(copy, args->capacity);
  for (size_t i = 0; i < args->length; ++i) {
    add_arg(copy, args->args[i]);
  }
  return copy;
}

void free_args(command_args* args) {
  for (size_t i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }
  free(args->args);
}
