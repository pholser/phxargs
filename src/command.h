#ifndef PHXARGS_COMMAND_H
#define PHXARGS_COMMAND_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "options.h"

typedef struct _command command;

command* command_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv);

uint8_t command_arg_would_exceed_limits(command* cmd, char* new_arg);

uint8_t command_should_execute_after_arg_added(command* cmd);

void command_replace_args(command* cmd, char* token);

void command_ensure_length_not_exceeded(command* cmd, char* new_arg);

pid_t command_execute_async(command* cmd);

uint8_t command_line_mode(command* cmd);

size_t command_max_length(command* cmd);

void command_add_input_argument(command* cmd, char* new_arg);

uint8_t command_input_args_remain(command* cmd);

size_t command_length(command* cmd);

void command_increment_line_count(command* cmd);

void command_free(command* cmd);

#endif  // PHXARGS_COMMAND_H
