#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

#include "options.h"

size_t parse_number_arg(int opt, const char* arg, char** endptr) {
  errno = 0;
  long parsed = strtol(arg, endptr, 10);

  if (arg == *endptr
    || errno != 0
    || (errno == 0 && **endptr != '\0')) {

    fprintf(stderr, "phxargs: -%c %s: Invalid\n", opt, arg);
    exit(EXIT_FAILURE);
  }
  if (parsed <= 0) {
    fprintf(stderr, "phxargs: -%c %s: too small\n", opt, arg);
  }

  return (size_t) parsed;
}

void options_disable_nul_char_as_arg_delimiter(options* const opts) {
  opts->use_nul_char_as_arg_delimiter = 0;
}

void options_reset_arg_delimiter(options* const opts) {
  opts->arg_delimiter = '\0';
}

void options_enable_nul_char_as_arg_delimiter(options* const opts) {
  opts->use_nul_char_as_arg_delimiter = 1;
  options_reset_arg_delimiter(opts);
}

void options_set_arg_delimiter(options* const opts, char ch) {
  opts->arg_delimiter = ch;
  options_disable_nul_char_as_arg_delimiter(opts);
}

void options_set_arg_file_path(options* const opts, char* path) {
  opts->arg_file_path = path;
}

void options_set_logical_end_of_input_marker(
  options* const opts,
  char* marker) {

  opts->logical_end_of_input_marker = marker;
}

void options_reset_logical_end_of_input_marker(options* const opts) {
  opts->logical_end_of_input_marker = NULL;
}

void options_reset_arg_placeholder(options* const opts) {
  opts->arg_placeholder = NULL;
}

void options_reset_max_lines_per_command(options* const opts) {
  opts->max_lines_per_command = 0;
  opts->max_lines_endptr = NULL;
}

void options_reset_max_args_per_command(options* const opts) {
  opts->max_args_per_command = 0;
  opts->max_args_endptr = NULL;
}

void options_set_arg_placeholder(options* const opts, char* placeholder) {
  opts->arg_placeholder = placeholder;

  options_reset_max_lines_per_command(opts);
  options_reset_max_args_per_command(opts);
}

void options_set_max_lines_per_command(
  options* opts,
  int opt,
  const char* new_val) {

  opts->max_lines_per_command =
    parse_number_arg(opt, new_val, &(opts->max_lines_endptr));

  options_reset_arg_placeholder(opts);
  options_reset_max_args_per_command(opts);
}

void options_set_max_args_per_command(
  options* opts,
  int opt,
  const char* new_val) {

  opts->max_args_per_command =
    parse_number_arg(opt, new_val, &(opts->max_args_endptr));

  options_reset_arg_placeholder(opts);
  options_reset_max_lines_per_command(opts);
}

void options_set_max_command_length(
  options* const opts,
  int opt,
  const char* new_val) {

  opts->max_command_length =
    parse_number_arg(opt, new_val, &(opts->max_command_length_endptr));
}

void options_enable_trace(options* const opts) {
  opts->trace = 1;
}

void options_enable_prompt(options* const opts) {
  opts->prompt = 1;
  options_enable_trace(opts);
}

void options_enable_terminate_on_too_large_command(options* const opts) {
  opts->terminate_on_too_large_command = 1;
}

uint8_t options_line_mode(const options* const opts) {
  return opts->max_lines_endptr != NULL;
}

uint8_t options_max_command_length_specified(const options* const opts) {
  return opts->max_command_length_endptr != NULL;
}

void init_options(options* const opts) {
  opts->use_nul_char_as_arg_delimiter = 0;
  opts->arg_file_path = NULL;
  opts->arg_delimiter = '\0';
  opts->logical_end_of_input_marker = NULL;
  options_reset_arg_placeholder(opts);
  options_reset_max_lines_per_command(opts);
  options_reset_max_args_per_command(opts);
  opts->prompt = 0;
  opts->max_command_length = 0;
  opts->max_command_length_endptr = NULL;
  opts->trace = 0;
  opts->terminate_on_too_large_command = 0;
}

int parse_options(options* const opts, int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, ":0a:d:E:I:L:n:ps:tx")) != -1) {
    switch (opt) {
      case '0':
        options_enable_nul_char_as_arg_delimiter(opts);
        options_reset_arg_delimiter(opts);
        break;
      case 'a':
        options_set_arg_file_path(opts, optarg);
        break;
      case 'd':
        if (strlen(optarg) > 1) {
          fprintf(stderr, "phxargs: invalid delimiter for -%c\n", optopt);
          exit(EXIT_FAILURE);
        }
        options_set_arg_delimiter(opts, *optarg);
        options_disable_nul_char_as_arg_delimiter(opts);
        break;
      case 'E':
        options_set_logical_end_of_input_marker(opts, optarg);
        break;
      case 'I':
        options_set_arg_placeholder(opts, optarg);
        break;
      case 'L':
        options_set_max_lines_per_command(opts, opt, optarg);
        break;
      case 'n':
        options_set_max_args_per_command(opts, opt, optarg);
        break;
      case 'p':
        options_enable_prompt(opts);
        break;
      case 's':
        options_set_max_command_length(opts, opt, optarg);
        break;
      case 't':
        options_enable_trace(opts);
        break;
      case 'x':
        options_enable_terminate_on_too_large_command(opts);
        break;
      case ':':
        fprintf(stderr, "phxargs: -%c needs an argument\n", optopt);
        exit(EXIT_FAILURE);
      case '?':
        fprintf(stderr, "phxargs: unknown option -%c\n", optopt);
        exit(EXIT_FAILURE);
      default:
        assert(!"reachable");
        exit(EXIT_FAILURE);
    }
  }

  /* -L implies -x */
  if (options_line_mode(opts)) {
    options_enable_terminate_on_too_large_command(opts);
  }

  return optind;
}
