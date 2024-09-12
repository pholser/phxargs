#include <stdlib.h>

#include "appender_mode.h"
#include "replacer_mode.h"
#include "util.h"
#include "xargs_mode.h"

void init_xargs_mode(
  xargs_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  if (opts->arg_placeholder == NULL) {
    mode->kind = ARG_APPENDER;
    mode->mode = safe_malloc(sizeof(appender_mode));
    init_appender_mode(mode->mode, opts, arg_index, argc, argv);
  } else {
    mode->kind = ARG_REPLACER;
    mode->mode = safe_malloc(sizeof(replacer_mode));
    init_replacer_mode(mode->mode, opts, arg_index, argc, argv);
  }
}

int run_xargs_in_mode(const xargs_mode* const mode) {
  int execution_status = EXIT_SUCCESS;

  switch (mode->kind) {
    case ARG_APPENDER:
      execution_status |=
        run_appender_mode((appender_mode*) mode->mode);
      break;
    case ARG_REPLACER:
      execution_status |=
        run_replacer_mode((replacer_mode*) mode->mode);
      break;
  }

  return execution_status;
}

void free_xargs_mode(const xargs_mode* const mode) {
  switch (mode->kind) {
    case ARG_APPENDER:
      free_appender_mode((appender_mode*) mode->mode);
      break;
    case ARG_REPLACER:
      free_replacer_mode((replacer_mode*) mode->mode);
      break;
  }
  free(mode->mode);
}
