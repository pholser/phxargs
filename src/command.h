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
  uint8_t prompt;
  uint8_t trace;

  uint8_t line_mode;
  size_t line_count;
  command_args* fixed_args;
  command_args* input_args;
} command;

void init_command(command* cmd, const options* opts);

uint8_t arg_would_exceed_limits(const command* cmd, const char* new_arg);

uint8_t should_execute_command(const command* cmd);

int execute_command(command* cmd);

void add_fixed_argument(const command* cmd, const char* new_arg);

void add_input_argument(const command* cmd, const char* new_arg);

size_t command_length(const command* cmd);

void free_command(const command* cmd);

#endif  // PHXARGS_COMMAND_H
