#ifndef PHXARGS_OPTIONS_H
#define PHXARGS_OPTIONS_H

#include <stddef.h>
#include <stdint.h>

typedef struct _options options;

options* options_create(int argc, char** argv);

void options_destroy(options* opts);

int options_optind(options* opts);

uint8_t options_use_nul_char_as_arg_delimiter(options* opts);

char* options_arg_file_path(options* opts);

char options_arg_delimiter(options* opts);

char* options_logical_end_of_input_marker(options* opts);

char* options_arg_placeholder(options* opts);

size_t options_max_lines_per_command(options* opts);

size_t options_max_args_per_command(options* opts);

uint8_t options_open_tty(options* opts);

uint8_t options_prompt(options* opts);

uint8_t options_suppress_execution_on_empty_input(options* opts);

size_t options_max_command_length(options* opts);

uint8_t options_trace(options* opts);

uint8_t options_terminate_on_too_large_command(options* opts);

uint8_t options_line_mode(options* opts);

uint8_t options_max_command_length_specified(options* opts);

#endif  // PHXARGS_OPTIONS_H
