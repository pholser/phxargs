#ifndef PHXARGS_COMMAND_ARGS_H
#define PHXARGS_COMMAND_ARGS_H

#include <stddef.h>

typedef struct command_args_s command_args;

command_args* command_args_create(void);

command_args* command_args_create_with_capacity(size_t capacity);

void command_args_add(command_args* args, const char* new_arg);

size_t command_args_count(const command_args* args);

size_t command_args_length(const command_args* args);

const char* command_args_at(const command_args* args, size_t i);

void command_args_destroy(command_args* args);

#endif // PHXARGS_COMMAND_ARGS_H
