#include <stdlib.h>

#include "appender_mode.h"
#include "util.h"
#include "xargs_mode.h"

struct _appender_mode {
  xargs_mode base;
  uint8_t suppress_execution_on_empty_input;
};

int appender_mode_run(xargs_mode* mode) {
  appender_mode* self = (appender_mode*) mode;

  int execution_status = EXIT_SUCCESS;
  uint8_t input_present = 0;

  char* token;
  for (token = xargs_mode_next_token(mode);
    token != NULL;
    token = xargs_mode_next_token(mode)) {

    input_present = 1;

    if (xargs_mode_arg_would_exceed_limits(mode, token)) {
      execution_status |= xargs_mode_execute_command(mode);
    }

    xargs_mode_add_input_argument(mode, token);
    if (xargs_mode_should_execute_command_after_arg_added(mode)) {
      execution_status |= xargs_mode_execute_command(mode);
    }
  }

  if (!input_present) {
    if (!self->suppress_execution_on_empty_input) {
      execution_status |= xargs_mode_execute_command(mode);
    }
  } else {
    if (xargs_mode_input_args_remain(mode)) {
      execution_status |= xargs_mode_execute_command(mode);
    }
  }

  return execution_status;
}

xargs_mode_ops appender_mode_ops = {
  .run = appender_mode_run
};

appender_mode* appender_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  appender_mode* mode = safe_malloc(sizeof(appender_mode));
  mode->suppress_execution_on_empty_input =
    opts->suppress_execution_on_empty_input;
  xargs_mode_init(
    &(mode->base),
    &appender_mode_ops,
    opts,
    arg_index,
    argc,
    argv);
  return mode;
}

void appender_mode_destroy(appender_mode* const mode) {
  xargs_mode_destroy(&(mode->base));
  free(mode);
}
