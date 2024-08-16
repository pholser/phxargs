#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>

#include "options.h"
#include "util.h"

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

void options_set_nul_char_as_argument_separator(options* opts) {
  opts->use_nul_char_as_arg_separator = true;
}

void options_set_logical_end_of_input_marker(options* opts, char* marker) {
  opts->logical_end_of_input_marker = marker;
}

void options_reset_max_lines_per_command(options* opts) {
  opts->max_lines_per_command = -1;
  opts->max_lines_endptr = NULL;
}

void options_reset_max_args_per_command(options* opts) {
  opts->max_args_per_command = 5000;
  opts->max_args_endptr = NULL;
}

void options_set_max_lines_per_command(options* opts, int opt, char* new_val) {
  opts->max_lines_per_command =
    parse_number_arg(opt, new_val, &(opts->max_lines_endptr));
  options_reset_max_args_per_command(opts);
}

void options_set_max_args_per_command(options* opts, int opt, char* new_val) {
  opts->max_args_per_command =
    parse_number_arg(opt, new_val, &(opts->max_args_endptr));
  options_reset_max_lines_per_command(opts);
}

void options_set_max_command_length(options* opts, int opt, char* new_val) {
  opts->max_command_length =
    parse_number_arg(opt, new_val, &(opts->max_command_length_endptr));
}

void options_enable_trace(options* opts) {
  opts->trace = true;
}

void options_enable_prompt(options* opts) {
  opts->prompt = true;
  options_enable_trace(opts);
}

void options_enable_terminate_on_too_large_command(options* opts) {
  opts->terminate_on_too_large_command = true;
}

void init_options(options* opts) {
  opts->use_nul_char_as_arg_separator = false;
  opts->logical_end_of_input_marker = NULL;
  options_reset_max_lines_per_command(opts);
  options_reset_max_args_per_command(opts);
  opts->prompt = false;
  opts->max_command_length = (size_t) (sysconf(_SC_ARG_MAX) - 4096);
  opts->max_command_length_endptr = NULL;
  opts->trace = false;
  opts->terminate_on_too_large_command = false;
}

int parse_options(options* opts, int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, ":0E:L:n:ps:tx")) != -1) {
    switch (opt) {
      case '0':
        options_set_nul_char_as_argument_separator(opts);
        break;
      case 'E':
        options_set_logical_end_of_input_marker(opts, optarg);
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
    }
  }

  return optind;
}
