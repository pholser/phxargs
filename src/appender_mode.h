#ifndef PHXARGS_APPENDER_MODE_H
#define PHXARGS_APPENDER_MODE_H

#include "options.h"

typedef struct _appender_mode appender_mode;

appender_mode* appender_mode_create(
  options* opts,
  int arg_index,
  int argc,
  char** argv);

void appender_mode_destroy(appender_mode* mode);

#endif  // PHXARGS_APPENDER_MODE_H
