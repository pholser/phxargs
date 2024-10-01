#ifndef PHXARGS_REPLACER_MODE_H
#define PHXARGS_REPLACER_MODE_H

#include "options.h"

typedef struct _replacer_mode replacer_mode;

replacer_mode* replacer_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv);

void replacer_mode_destroy(replacer_mode* mode);

#endif  // PHXARGS_REPLACER_MODE_H
