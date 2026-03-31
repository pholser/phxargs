#ifndef PHXARGS_COMMAND_H
#define PHXARGS_COMMAND_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include "options.h"

typedef struct command_s command;

command* command_create(const options* opts, int arg_index, int argc, const char* const* argv);

bool command_arg_would_exceed_limits(const command* cmd, const char* new_arg);

bool command_should_execute_after_arg_added(const command* cmd);

void command_replace_args(command* cmd, const char* token);

void command_ensure_length_not_exceeded(
  const command* cmd, const char* new_arg);

pid_t command_execute_async(command* cmd);

size_t command_max_length(const command* cmd);

void command_add_input_argument(command* cmd, const char* new_arg);

bool command_input_args_remain(const command* cmd);

size_t command_length(const command* cmd);

void command_increment_line_count(command* cmd);

void command_destroy(command* cmd);

#endif // PHXARGS_COMMAND_H
