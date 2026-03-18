#include <stdlib.h>

#include "arg_source.h"
#include "command.h"
#include "delim_tokenizer.h"
#include "options.h"
#include "space_tokenizer.h"
#include "tokenizer.h"
#include "xargs_mode.h"

int xargs_mode_run(xargs_mode* mode) {
  return mode->ops->run(mode);
}

char* xargs_mode_next_token(xargs_mode* const mode) {
  return tokenizer_next_token(
    mode->toker,
    mode->arg_source,
    mode->cmd);
}

uint8_t xargs_mode_arg_would_exceed_limits(
  xargs_mode* mode,
  char* new_arg) {

  return command_arg_would_exceed_limits(mode->cmd, new_arg);
}

uint8_t xargs_mode_should_execute_command_after_arg_added(
  xargs_mode* mode) {

  return command_should_execute_after_arg_added(mode->cmd);
}

int xargs_mode_execute_command(xargs_mode* mode) {
  return command_execute(mode->cmd);
}

void xargs_mode_add_input_argument(
  xargs_mode* mode,
  char* new_arg) {

  command_add_input_argument(mode->cmd, new_arg);
}

uint8_t xargs_mode_input_args_remain(xargs_mode* mode) {
  return command_input_args_remain(mode->cmd);
}

void xargs_mode_ensure_command_length_not_exceeded(
  xargs_mode* mode,
  char* new_arg) {

  return command_ensure_length_not_exceeded(mode->cmd, new_arg);
}

void xargs_replace_args(xargs_mode* mode, char* new_arg) {
  command_replace_args(mode->cmd, new_arg);
}

void xargs_mode_init(
  xargs_mode* mode,
  xargs_mode_ops* ops,
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  mode->ops = ops;
  mode->arg_source = arg_source_init(options_arg_file_path(opts));
  mode->cmd = command_create(opts, arg_index, argc, argv);

  if (options_use_nul_char_as_arg_delimiter(opts) || options_arg_delimiter(opts) != '\0') {
    mode->toker =
      (tokenizer*) delim_tokenizer_create(
        command_max_length(mode->cmd),
        options_arg_delimiter(opts));
  } else {
    mode->toker =
      (tokenizer*) space_tokenizer_create(
        command_max_length(mode->cmd),
        command_line_mode(mode->cmd),
        options_logical_end_of_input_marker(opts));
  }
}

void xargs_mode_destroy(xargs_mode* mode) {
  command_free(mode->cmd);
  tokenizer_destroy(mode->toker);
  free(mode);
}
