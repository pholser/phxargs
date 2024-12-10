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
    &(mode->cmd));
}

uint8_t xargs_mode_arg_would_exceed_limits(
  xargs_mode* mode,
  char* new_arg) {

  return command_arg_would_exceed_limits(&(mode->cmd), new_arg);
}

uint8_t xargs_mode_should_execute_command_after_arg_added(
  xargs_mode* mode) {

  return command_should_execute_after_arg_added(&(mode->cmd));
}

int xargs_mode_execute_command(xargs_mode* mode) {
  return command_execute(&(mode->cmd));
}

void xargs_mode_add_input_argument(
  xargs_mode* mode,
  char* new_arg) {

  command_add_input_argument(&(mode->cmd), new_arg);
}

uint8_t xargs_mode_input_args_remain(xargs_mode* mode) {
  return mode->cmd.input_args.count > 0;
}

void xargs_mode_ensure_command_length_not_exceeded(
  xargs_mode* mode,
  char* new_arg) {

  return command_ensure_length_not_exceeded(&(mode->cmd), new_arg);
}

void xargs_replace_args(xargs_mode* mode, char* new_arg) {
  command_replace_args(&(mode->cmd), new_arg);  
}

void xargs_mode_init(
  xargs_mode* mode,
  xargs_mode_ops* ops,
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  mode->ops = ops;
  mode->arg_source = arg_source_init(opts->arg_file_path);
  command_init(&(mode->cmd), opts, arg_index, argc, argv);

  if (opts->use_nul_char_as_arg_delimiter || opts->arg_delimiter != '\0') {
    mode->toker =
      (tokenizer*) delim_tokenizer_create(
        command_max_length(&(mode->cmd)),
        opts->arg_delimiter);
  } else {
    mode->toker =
      (tokenizer*) space_tokenizer_create(
        command_max_length(&(mode->cmd)),
        command_line_mode(&(mode->cmd)),
        opts->logical_end_of_input_marker);
  }
}

void xargs_mode_destroy(xargs_mode* mode) {
  tokenizer_destroy(mode->toker);
  free(mode);
}
