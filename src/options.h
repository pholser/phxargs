#ifndef PHXARGS_OPTIONS_H
#define PHXARGS_OPTIONS_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  /* -0 option */
  uint8_t use_nul_char_as_arg_delimiter;

  /* -a option */
  char* arg_file_path;

  /* -d option */
  char arg_delimiter;

  /* -E option */
  char* logical_end_of_input_marker;

  /* -I option */
  uint8_t arg_placeholder_enabled;
  char* arg_placeholder;

  /* -L option */
  size_t max_lines_per_command;
  char* max_lines_endptr;

  /* -n option */
  size_t max_args_per_command;
  char* max_args_endptr;

  /* -p option */
  uint8_t prompt;

  /* -s option */
  size_t max_command_length;
  char* max_command_length_endptr;

  /* -t option */
  uint8_t trace;

  /* -x option */
  uint8_t terminate_on_too_large_command;
} options;

void init_options(options* const opts);

int parse_options(options* const opts, int argc, char** argv);

uint8_t options_line_mode(const options* const opts);

uint8_t options_max_command_length_specified(const options* const opts);

#endif  // PHXARGS_OPTIONS_H
