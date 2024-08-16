#include <stdlib.h>
#include <string.h>

#include "command_args.h"
#include "util.h"

command_args* allocate_args() {
  command_args* args = safe_malloc(sizeof(command_args));

  args->count = 0;
  args->capacity = 10;
  args->args = safe_malloc(args->capacity * sizeof(char*));
  args->length = 0;

  return args;
}

void reallocate_args_if_needed(command_args* args) {
  if (args->count >= args->capacity) {
    args->capacity *= 2;
    args->args =
      safe_realloc(args->args, args->capacity * sizeof(char*));
  }
}

void add_arg(command_args* args, const char* new_arg) {
  reallocate_args_if_needed(args);

  args->args[args->count] = strdup(new_arg);
  ++args->count;
  args->length += strlen(new_arg) + 1;
}

void free_args(const command_args* args) {
  for (size_t i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }
  free(args->args);
}
