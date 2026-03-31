#ifndef PHXARGS_REPLACER_MODE_H
#define PHXARGS_REPLACER_MODE_H

#include "options.h"
#include "xargs_mode.h"

typedef struct replacer_mode_s replacer_mode;

replacer_mode* replacer_mode_create(
  options* opts,
  int arg_index,
  int argc,
  const char* const* argv);

xargs_mode* replacer_mode_as_xargs_mode(replacer_mode* mode);

void replacer_mode_destroy(replacer_mode* mode);

#endif // PHXARGS_REPLACER_MODE_H
