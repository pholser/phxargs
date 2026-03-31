#include "command_args.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

struct command_args_s {
  size_t count;
  size_t capacity;
  char** args;
  size_t length;
};

static void reallocate_if_needed(command_args* args) {
  if (args->count >= args->capacity) {
    args->capacity *= 2;
    args->args =
      (char**) safe_realloc(
        (void*) args->args,
        args->capacity * sizeof(char*));
  }
}

command_args* command_args_create(void) {
  return command_args_create_with_capacity(10);
}

command_args* command_args_create_with_capacity(size_t capacity) {
  command_args* args = safe_malloc(sizeof(command_args));

  args->count = 0;
  args->capacity = capacity;
  args->args = (char**) safe_calloc(args->capacity, sizeof(char*));
  args->length = 0;

  return args;
}

void command_args_add(command_args* args, const char* new_arg) {
  reallocate_if_needed(args);
  args->args[args->count++] = safe_strdup(new_arg);
  args->length += strlen(new_arg) + 1;
}

size_t command_args_count(const command_args* args) {
  return args->count;
}

size_t command_args_length(const command_args* args) {
  return args->length;
}

char* command_args_at(const command_args* args, size_t i) {
  return args->args[i];
}

void command_args_destroy(command_args* args) {
  for (size_t i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }

  free((void*) args->args);
  free(args);
}
