#include <stdlib.h>

#include "options.h"
#include "replacer_mode.h"
#include "util.h"
#include "xargs_mode.h"

struct _replacer_mode {
  xargs_mode base;

  char* placeholder;
};

int replacer_mode_run(xargs_mode* mode) {
  replacer_mode* self = (replacer_mode*) mode;

  int execution_status = EXIT_SUCCESS;

  char* token;
  for (token = xargs_mode_next_token(&(self->base));
    token != NULL;
    token = xargs_mode_next_token(&(self->base))) {

    xargs_mode_ensure_command_length_not_exceeded(&(self->base), token);
    xargs_replace_args(&(self->base), token);

    execution_status |= xargs_mode_execute_command(&(self->base));
 }

  return execution_status;
}

xargs_mode_ops replacer_mode_ops = {
  .run = replacer_mode_run
};

replacer_mode* replacer_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv) {

  replacer_mode* mode = safe_malloc(sizeof(replacer_mode));
  xargs_mode_init(
    &(mode->base),
    &replacer_mode_ops,
    opts,
    arg_index,
    argc,
    argv);
  mode->placeholder = safe_strdup(opts->arg_placeholder);
  return mode;
}

void replacer_mode_destroy(replacer_mode* mode) {
  free(mode->placeholder);
  xargs_mode_destroy(&(mode->base));
  free(mode);
}
