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

void init_command(
  command* const cmd,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv);

uint8_t arg_would_exceed_limits(const command* const cmd, const char* new_arg);

uint8_t should_execute_command_after_arg_added(const command* const cmd);

void command_replace_args(command* const cmd, const char* const token);

void command_ensure_length_not_exceeded(
  const command* const cmd,
  const char* const new_arg);

int execute_command(command* const cmd);

void add_input_argument(command* const cmd, const char* const new_arg);

size_t command_length(const command* const cmd);

void free_command(const command* const cmd);

#endif  // PHXARGS_COMMAND_H
