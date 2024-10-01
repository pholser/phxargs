#include <stdlib.h>

#include "appender_mode.h"
#include "util.h"
#include "xargs_mode.h"

struct _appender_mode {
  xargs_mode base;
};

int appender_mode_run(xargs_mode* mode) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  for (token = xargs_mode_next_token(mode);
    token != NULL;
    token = xargs_mode_next_token(mode)) {

    if (xargs_mode_arg_would_exceed_limits(mode, token)) {
      execution_status |= xargs_mode_execute_command(mode);
    }

    xargs_mode_add_input_argument(mode, token);
    if (xargs_mode_should_execute_command_after_arg_added(mode)) {
      execution_status |= xargs_mode_execute_command(mode);
    }
  }

  if (xargs_mode_input_args_remain(mode)) {
    execution_status |= xargs_mode_execute_command(mode);
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
