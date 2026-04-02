#include "xargs_mode.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "arg_source.h"
#include "command.h"
#include "delim_tokenizer.h"
#include "options.h"
#include "process_pool.h"
#include "space_tokenizer.h"
#include "tokenizer.h"
#include "util.h"

static void increment_line_count(void* ctx) {
  command_increment_line_count((command*) ctx);
}

struct xargs_mode_s {
  const xargs_mode_ops* ops;
  FILE* arg_source;
  command* cmd;
  tokenizer* toker;
  process_pool* pool;
  void* impl;
};

xargs_mode* xargs_mode_create(
  const xargs_mode_ops* ops,
  const options* opts,
  int arg_index,
  int argc,
  const char* const* argv,
  void* impl) {

  xargs_mode* mode = phxargs_malloc(sizeof(xargs_mode));

  mode->ops = ops;
  mode->impl = impl;
  mode->arg_source = arg_source_open(options_arg_file_path(opts));
  mode->cmd = command_create(opts, arg_index, argc, argv);
  mode->pool = process_pool_create(options_max_procs(opts));

  if (options_use_nul_char_as_arg_delimiter(opts)
    || options_arg_delimiter(opts) != '\0') {

    mode->toker = delim_tokenizer_as_tokenizer(delim_tokenizer_create(
      command_max_length(mode->cmd),
      options_arg_delimiter(opts),
      increment_line_count,
      mode->cmd));
  } else {
    mode->toker = space_tokenizer_as_tokenizer(space_tokenizer_create(
      command_max_length(mode->cmd),
      options_logical_end_of_input_marker(opts),
      increment_line_count,
      mode->cmd));
  }

  return mode;
}

void* xargs_mode_impl(const xargs_mode* mode) {
  return mode->impl;
}

int xargs_mode_run(xargs_mode* mode) {
  return mode->ops->run(mode);
}

const char* xargs_mode_next_token(xargs_mode* const mode) {
  return tokenizer_next_token(mode->toker, mode->arg_source);
}

bool xargs_mode_tokenizer_errored(const xargs_mode* mode) {
  return tokenizer_get_error(mode->toker) != TOKENIZER_ERR_NONE;
}

bool xargs_mode_arg_would_exceed_limits(xargs_mode* mode, const char* new_arg) {
  return command_arg_would_exceed_limits(mode->cmd, new_arg);
}

bool xargs_mode_should_execute_command_after_arg_added(xargs_mode* mode) {
  int result = command_should_execute_after_arg_added(mode->cmd);
  if (result < 0) {
    fprintf(stderr, "phxargs: command too long\n");
    process_pool_drain(mode->pool);
    exit(EXIT_FAILURE);
  }

  return result > 0;
}

void xargs_mode_execute_command(xargs_mode* mode) {
  process_pool_wait_if_full(mode->pool);

  if (process_pool_halted(mode->pool)) {
    return;
  }

  process_pool_submit(mode->pool, command_execute_async(mode->cmd));
}

int xargs_mode_drain(xargs_mode* mode) {
  return process_pool_drain(mode->pool);
}

void xargs_mode_add_input_argument(xargs_mode* mode, const char* new_arg) {
  command_add_input_argument(mode->cmd, new_arg);
}

bool xargs_mode_input_args_remain(const xargs_mode* mode) {
  return command_input_args_remain(mode->cmd);
}

void xargs_mode_ensure_command_length_not_exceeded(
  xargs_mode* mode, const char* new_arg) {
  command_ensure_length_not_exceeded(mode->cmd, new_arg);
}

void xargs_replace_args(xargs_mode* mode, const char* new_arg) {
  command_replace_args(mode->cmd, new_arg);
}

void xargs_mode_destroy(xargs_mode* mode) {
  mode->ops->destroy_impl(mode->impl);
  command_destroy(mode->cmd);
  tokenizer_destroy(mode->toker);
  process_pool_destroy(mode->pool);
  arg_source_close(mode->arg_source);
  free(mode);
}
