#include "appender_mode.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "options.h"
#include "util.h"
#include "xargs_mode.h"

struct appender_mode_s {
  xargs_mode* base;
  uint8_t suppress_execution_on_empty_input;
};

/*
 * As long as there are more arguments to read ...
 * (1) read the next arg, `x`
 * (2) after having read `x`:
 *   (a) adding `x` to an empty cmd may make command impossible
 *       due to overrunning. Fail it.
 *   (b) in max-args mode, we may be at max args before adding
 *       `x`. Execute cmd.
 *   (c) in line mode, we may have in the act of reading `x`
 *       incremented line count to the max. Execute cmd.
 *   (d) we may be at a point where adding `x` would cause us
 *       to exceed implied or explicit size bound. Execute cmd.
 *
 * (3) Add `x` to cmd.
 */
int appender_mode_run(xargs_mode* mode) {
  appender_mode* self = (appender_mode*) xargs_mode_impl(mode);

  int execution_status = EXIT_SUCCESS;
  bool input_present = false;

  char* token;
  for (token = xargs_mode_next_token(mode);
    token != NULL;
    token = xargs_mode_next_token(mode)) {

    input_present = true;

    if (xargs_mode_arg_would_exceed_limits(mode, token)) {
      xargs_mode_execute_command(mode);
    }

    xargs_mode_add_input_argument(mode, token);
    if (xargs_mode_should_execute_command_after_arg_added(mode)) {
      xargs_mode_execute_command(mode);
    }
  }

  if (xargs_mode_tokenizer_errored(mode)) {
    xargs_mode_drain(mode);
    exit(EXIT_FAILURE);
  }

  if (!input_present) {
    if (!self->suppress_execution_on_empty_input) {
      xargs_mode_execute_command(mode);
    }
  } else {
    if (xargs_mode_input_args_remain(mode)) {
      xargs_mode_execute_command(mode);
    }
  }

  execution_status |= xargs_mode_drain(mode);
  return execution_status;
}

static void appender_mode_destroy_impl(void* impl) {
  free(impl);
}

static xargs_mode_ops appender_mode_ops = {
  .run = appender_mode_run,
  .destroy_impl = appender_mode_destroy_impl
};

appender_mode* appender_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  appender_mode* mode = safe_malloc(sizeof(appender_mode));

  mode->suppress_execution_on_empty_input =
    options_suppress_execution_on_empty_input(opts);
  mode->base =
    xargs_mode_create(&appender_mode_ops, opts, arg_index, argc, argv, mode);

  return mode;
}

xargs_mode* appender_mode_as_xargs_mode(appender_mode* mode) {
  return mode->base;
}

void appender_mode_destroy(appender_mode* mode) {
  xargs_mode_destroy(mode->base);
}
