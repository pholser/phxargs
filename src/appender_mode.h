#ifndef PHXARGS_APPENDER_MODE_H
#define PHXARGS_APPENDER_MODE_H

#include "options.h"
#include "xargs_mode.h"

typedef struct appender_mode_s appender_mode;

appender_mode* appender_mode_create(
  const options* opts,
  int arg_index,
  int argc,
  const char* const* argv);

xargs_mode* appender_mode_as_xargs_mode(const appender_mode* mode);

void appender_mode_destroy(appender_mode* mode);

#endif // PHXARGS_APPENDER_MODE_H
