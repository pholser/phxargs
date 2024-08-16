#ifndef PHXARGS_COMMAND_H
#define PHXARGS_COMMAND_H

#include "command_args.h"
#include "options.h"

typedef struct {
  size_t max_lines;
  size_t max_args;
  size_t max_length;
  bool prompt;
  bool trace;

  size_t line_count;
  command_args* fixed_args;
  command_args* input_args;
} command;

void init_command(command* cmd, const options* opts);

bool arg_would_exceed_limits(const command* cmd, const char* new_arg);

bool should_execute_command(const command* cmd);

int execute_command(command* cmd);

void add_fixed_argument(const command* cmd, const char* new_arg);

void add_input_argument(const command* cmd, const char* new_arg);

size_t command_length(const command* cmd);

void free_command(const command* cmd);

#endif  // PHXARGS_COMMAND_H
