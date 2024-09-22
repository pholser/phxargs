#ifndef PHXARGS_APPENDER_MODE_H
#define PHXARGS_APPENDER_MODE_H

#include <stdio.h>

#include "command.h"
#include "options.h"
#include "tokenizer.h"


typedef struct {
  command cmd;
  tokenizer toker;
  FILE* arg_source;
} appender_mode;

void init_appender_mode(
  appender_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv);

int run_appender_mode(appender_mode* const mode);

void free_appender_mode(const appender_mode* const mode);

#endif  // PHXARGS_APPENDER_MODE_H
