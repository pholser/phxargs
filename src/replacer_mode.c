#include <stdlib.h>

#include "options.h"
#include "replacer_mode.h"
#include "util.h"
#include "xargs_mode.h"

struct _replacer_mode {
  xargs_mode* base;
  char* placeholder;
};

int replacer_mode_run(xargs_mode* mode) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  for (token = xargs_mode_next_token(mode);
    token != NULL;
    token = xargs_mode_next_token(mode)) {

    xargs_mode_ensure_command_length_not_exceeded(mode, token);
    xargs_replace_args(mode, token);

    xargs_mode_execute_command(mode);
  }

  execution_status |= xargs_mode_drain(mode);
  return execution_status;
}

static void replacer_mode_destroy_impl(void* impl) {
  replacer_mode* mode = (replacer_mode*) impl;
  free(mode->placeholder);
  free(mode);
}

xargs_mode_ops replacer_mode_ops = {
  .run = replacer_mode_run,
  .destroy_impl = replacer_mode_destroy_impl
};

replacer_mode* replacer_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  replacer_mode* mode = safe_malloc(sizeof(replacer_mode));
  mode->placeholder = safe_strdup(options_arg_placeholder(opts));
  mode->base = xargs_mode_create(
    &replacer_mode_ops,
    opts,
    arg_index,
    argc,
    argv,
    mode);
  return mode;
}

xargs_mode* replacer_mode_base(replacer_mode* mode) {
  return mode->base;
}

void replacer_mode_destroy(replacer_mode* mode) {
  xargs_mode_destroy(mode->base);
}
