#ifndef PHXARGS_XARGS_MODE_H
#define PHXARGS_XARGS_MODE_H

#include "options.h"

typedef enum {
  ARG_APPENDER,
  ARG_REPLACER
} xargs_mode_kind;

typedef struct {
  xargs_mode_kind kind;
  void* mode;
} xargs_mode;

void init_xargs_mode(
  xargs_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv);

int run_xargs_in_mode(const xargs_mode* const mode);

void free_xargs_mode(const xargs_mode* const mode);

#endif  // PHXARGS_XARGS_MODE_H
