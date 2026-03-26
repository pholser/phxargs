#ifndef PHXARGS_OPTIONS_H
#define PHXARGS_OPTIONS_H

#include <stddef.h>
#include <stdint.h>

typedef struct _options options;

options* options_create(int argc, char** argv);

void options_destroy(options* opts);

int options_index_of_first_arg(const options* opts);

uint8_t options_use_nul_char_as_arg_delimiter(const options* opts);

const char* options_arg_file_path(const options* opts);

char options_arg_delimiter(const options* opts);

const char* options_logical_end_of_input_marker(const options* opts);

const char* options_arg_placeholder(const options* opts);

size_t options_max_lines_per_command(const options* opts);

size_t options_max_args_per_command(const options* opts);

uint8_t options_open_tty(const options* opts);

uint8_t options_prompt(const options* opts);

size_t options_max_procs(const options* opts);

uint8_t options_suppress_execution_on_empty_input(const options* opts);

size_t options_max_command_length(const options* opts);

uint8_t options_trace(const options* opts);

uint8_t options_terminate_on_too_large_command(const options* opts);

uint8_t options_line_mode(const options* opts);

uint8_t options_max_command_length_specified(const options* opts);

#endif  // PHXARGS_OPTIONS_H
