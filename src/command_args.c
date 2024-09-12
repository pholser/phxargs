#include <stdlib.h>
#include <string.h>

#include "command_args.h"
#include "util.h"

void init_args_with_capacity(command_args* const args, size_t capacity) {
  args->count = 0;
  args->capacity = capacity;
  args->args = safe_calloc(args->capacity, sizeof(char*));
  args->length = 0;
}

void init_args(command_args* const args) {
  init_args_with_capacity(args, 10);
}

void reallocate_args_if_needed(command_args* const args) {
  if (args->count + 1 > args->capacity) {
    args->capacity *= 2;
    args->args =
      safe_realloc(args->args, args->capacity * sizeof(char*));
  }
}

void add_arg(command_args* const args, const char* const new_arg) {
  reallocate_args_if_needed(args);

  args->args[args->count++] = safe_strdup(new_arg);
  args->length += strlen(new_arg) + 1;
}

void free_args(const command_args* const args) {
  for (size_t i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }
  free(args->args);
}
