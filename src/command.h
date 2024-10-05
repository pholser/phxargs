#ifndef PHXARGS_COMMAND_H
#define PHXARGS_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "command_args.h"
#include "options.h"

typedef struct {
  size_t max_lines;
  size_t max_args;
  size_t max_length;
  char* arg_placeholder;
  uint8_t open_tty;
  uint8_t prompt;
  uint8_t trace;
  uint8_t terminate_on_too_large_command;
  uint8_t line_mode;

  size_t line_count;
  size_t env_length;
  command_args fixed_args;
  command_args input_args;
  command_args replaced_fixed_args;
} command;

void command_init(
  command* cmd,
  options* opts,
  int arg_index,
  int argc,
  char** argv);

uint8_t command_arg_would_exceed_limits(command* cmd, char* new_arg);

uint8_t command_should_execute_after_arg_added(command* cmd);

void command_replace_args(command* cmd, char* token);

void command_ensure_length_not_exceeded(
  command* cmd,
  char* new_arg);

int command_execute(command* cmd);

uint8_t command_line_mode(command* cmd);

size_t command_max_length(command* cmd);

void command_add_input_argument(command* cmd, char* new_arg);

uint8_t command_input_args_remain(command* cmd);

size_t command_length(command* cmd);

void command_free(command* cmd);

#endif  // PHXARGS_COMMAND_H
