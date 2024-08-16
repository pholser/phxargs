#ifndef PHXARGS_OPTIONS_H
#define PHXARGS_OPTIONS_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  /* -0 option */
  bool use_nul_char_as_arg_separator;

  /* -E option */
  char* logical_end_of_input_marker;

  /* -L option */
  size_t max_lines_per_command;
  char* max_lines_endptr;

  /* -n option */
  size_t max_args_per_command;
  char* max_args_endptr;

  /* -p option */
  bool prompt;

  /* -s option */
  size_t max_command_length;
  char* max_command_length_endptr;

  /* -t option */
  bool trace;

  /* -x option */
  bool terminate_on_too_large_command;
} options;

void init_options(options* opts);

int parse_options(options* opts, int argc, char** argv);

#endif  // PHXARGS_OPTIONS_H
