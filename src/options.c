#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

#include "options.h"
#include "util.h"

struct _options {
  int optind_val;

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

  /* -o option */
  uint8_t open_tty;

  /* -p option */
  uint8_t prompt;

  /* -r option */
  uint8_t suppress_execution_on_empty_input;

  /* -s option */
  size_t max_command_length;
  char* max_command_length_endptr;

  /* -t option */
  uint8_t trace;

  /* -x option */
  uint8_t terminate_on_too_large_command;

  /* -P option */
  size_t max_procs;
  char* max_procs_endptr;
};

static long parse_number_arg_with_min(int opt, const char* arg, char** endptr, long min) {
  errno = 0;
  long parsed = strtol(arg, endptr, 10);

  if (arg == *endptr || errno != 0 || **endptr != '\0') {
    fprintf(stderr, "phxargs: -%c %s: Invalid\n", opt, arg);
    exit(EXIT_FAILURE);
  }
  if (parsed < min) {
    fprintf(stderr, "phxargs: -%c %s: too small\n", opt, arg);
    exit(EXIT_FAILURE);
  }

  return parsed;
}

static size_t parse_number_arg(int opt, const char* arg, char** endptr) {
  return (size_t) parse_number_arg_with_min(opt, arg, endptr, 1);
}

static void disable_nul_char_as_arg_delimiter(options* opts) {
  opts->use_nul_char_as_arg_delimiter = 0;
}

static void reset_arg_delimiter(options* opts) {
  opts->arg_delimiter = '\0';
}

static void enable_nul_char_as_arg_delimiter(options* opts) {
  opts->use_nul_char_as_arg_delimiter = 1;
  reset_arg_delimiter(opts);
}

static void set_arg_delimiter(options* opts, char ch) {
  opts->arg_delimiter = ch;
  disable_nul_char_as_arg_delimiter(opts);
}

static void set_arg_file_path(options* opts, char* path) {
  opts->arg_file_path = path;
}

static void set_logical_end_of_input_marker(options* opts, char* marker) {
  opts->logical_end_of_input_marker = marker;
}

static void reset_logical_end_of_input_marker(options* opts) {
  opts->logical_end_of_input_marker = NULL;
}

static void reset_arg_placeholder(options* opts) {
  opts->arg_placeholder_enabled = 0;
  opts->arg_placeholder = NULL;
}

static void reset_max_lines_per_command(options* opts) {
  opts->max_lines_per_command = 0;
  opts->max_lines_endptr = NULL;
}

static void reset_max_args_per_command(options* opts) {
  opts->max_args_per_command = 0;
  opts->max_args_endptr = NULL;
}

static void set_max_lines_per_command(
  options* opts,
  int opt,
  const char* new_val) {

  opts->max_lines_per_command =
    parse_number_arg(opt, new_val, &(opts->max_lines_endptr));

  reset_arg_placeholder(opts);
  reset_max_args_per_command(opts);
}

static void set_max_args_per_command(
  options* opts,
  int opt,
  const char* new_val) {

  opts->max_args_per_command =
    parse_number_arg(opt, new_val, &(opts->max_args_endptr));

  reset_arg_placeholder(opts);
  reset_max_lines_per_command(opts);
}

static void set_max_command_length(
  options* const opts,
  int opt,
  const char* new_val) {

  opts->max_command_length =
    parse_number_arg(opt, new_val, &(opts->max_command_length_endptr));
}

static void set_max_procs(options* opts, int opt, const char* new_val) {
  long parsed =
    parse_number_arg_with_min(opt, new_val, &(opts->max_procs_endptr), 0);

  if (parsed > 0) {
    long child_max = sysconf(_SC_CHILD_MAX);
    if (child_max > 0 && parsed > child_max) {
      fprintf(
        stderr,
        "phxargs: -P %zu: too large (system limit is %ld)\n",
        (size_t) parsed,
        child_max);
      exit(EXIT_FAILURE);
    }
  }

  opts->max_procs = (size_t) parsed;
}

static void set_arg_placeholder(options* opts, char* placeholder) {
  opts->arg_placeholder_enabled = 1;
  opts->arg_placeholder = placeholder;

  reset_max_lines_per_command(opts);
  reset_max_args_per_command(opts);
}

static void set_open_tty(options* opts) {
  opts->open_tty = 1;
}

static void enable_trace(options* opts) {
  opts->trace = 1;
}

static void enable_prompt(options* opts) {
  opts->prompt = 1;
  enable_trace(opts);
}

static void enable_suppress_execution_on_empty_input(options* opts) {
  opts->suppress_execution_on_empty_input = 1;
}

static void enable_terminate_on_too_large_command(options* opts) {
  opts->terminate_on_too_large_command = 1;
}

static void init_options(options* opts) {
  opts->optind_val = 0;
  opts->use_nul_char_as_arg_delimiter = 0;
  opts->arg_file_path = NULL;
  opts->arg_delimiter = '\0';
  opts->logical_end_of_input_marker = NULL;
  reset_arg_placeholder(opts);
  reset_max_lines_per_command(opts);
  reset_max_args_per_command(opts);
  opts->open_tty = 0;
  opts->prompt = 0;
  opts->max_procs = 1;
  opts->max_procs_endptr = NULL;
  opts->max_command_length = 0;
  opts->max_command_length_endptr = NULL;
  opts->suppress_execution_on_empty_input = 0;
  opts->trace = 0;
  opts->terminate_on_too_large_command = 0;
}

static void parse_options(options* opts, int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, ":0a:d:E:I:L:n:opP:rs:tx")) != -1) {
    switch (opt) {
      case '0':
        enable_nul_char_as_arg_delimiter(opts);
        break;
      case 'a':
        set_arg_file_path(opts, optarg);
        break;
      case 'd':
        if (strlen(optarg) > 1) {
          fprintf(stderr, "phxargs: invalid delimiter for -%c\n", optopt);
          exit(EXIT_FAILURE);
        }
        set_arg_delimiter(opts, *optarg);
        break;
      case 'E':
        set_logical_end_of_input_marker(opts, optarg);
        break;
      case 'I':
        set_arg_placeholder(opts, optarg);
        break;
      case 'L':
        set_max_lines_per_command(opts, opt, optarg);
        break;
      case 'n':
        set_max_args_per_command(opts, opt, optarg);
        break;
      case 'o':
        set_open_tty(opts);
        break;
      case 'p':
        enable_prompt(opts);
        break;
      case 'P':
        set_max_procs(opts, opt, optarg);
        break;
      case 'r':
        enable_suppress_execution_on_empty_input(opts);
        break;
      case 's':
        set_max_command_length(opts, opt, optarg);
        break;
      case 't':
        enable_trace(opts);
        break;
      case 'x':
        enable_terminate_on_too_large_command(opts);
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
}

static void configure_options(options* opts, int argc, char** argv) {
  parse_options(opts, argc, argv);

  /* -L implies -x */
  if (options_line_mode(opts)) {
    enable_terminate_on_too_large_command(opts);
  }

  /* -I implies -x */
  /* -I basically nullifies -E */
  /* -I implies -L 1, and uses '\n' as delimiter if no other in force */
  if (opts->arg_placeholder_enabled) {
    enable_terminate_on_too_large_command(opts);
    reset_logical_end_of_input_marker(opts);

    opts->max_lines_per_command = 1;

    if (opts->arg_delimiter == '\0' || !opts->use_nul_char_as_arg_delimiter) {
      set_arg_delimiter(opts, '\n');
    }
  }

  opts->optind_val = optind;
}

options* options_create(int argc, char** argv) {
  options* opts = safe_malloc(sizeof(options));

  init_options(opts);
  configure_options(opts, argc, argv);

  return opts;
}

void options_destroy(options* opts) {
  free(opts);
}

int options_optind(options* opts) {
  return opts->optind_val;
}

uint8_t options_use_nul_char_as_arg_delimiter(options* opts) {
  return opts->use_nul_char_as_arg_delimiter;
}

char* options_arg_file_path(options* opts) {
  return opts->arg_file_path;
}

char options_arg_delimiter(options* opts) {
  return opts->arg_delimiter;
}

char* options_logical_end_of_input_marker(options* opts) {
  return opts->logical_end_of_input_marker;
}

char* options_arg_placeholder(options* opts) {
  return opts->arg_placeholder;
}

size_t options_max_lines_per_command(options* opts) {
  return opts->max_lines_per_command;
}

size_t options_max_args_per_command(options* opts) {
  return opts->max_args_per_command;
}

uint8_t options_open_tty(options* opts) {
  return opts->open_tty;
}

uint8_t options_prompt(options* opts) {
  return opts->prompt;
}

size_t options_max_procs(options* opts) {
  return opts->max_procs;
}

uint8_t options_suppress_execution_on_empty_input(options* opts) {
  return opts->suppress_execution_on_empty_input;
}

size_t options_max_command_length(options* opts) {
  return opts->max_command_length;
}

uint8_t options_trace(options* opts) {
  return opts->trace;
}

uint8_t options_terminate_on_too_large_command(options* opts) {
  return opts->terminate_on_too_large_command;
}

uint8_t options_line_mode(options* opts) {
  return opts->max_lines_endptr != NULL;
}

uint8_t options_max_command_length_specified(options* opts) {
  return opts->max_command_length_endptr != NULL;
}
