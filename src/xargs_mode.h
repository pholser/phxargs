#ifndef PHXARGS_XARGS_MODE_H
#define PHXARGS_XARGS_MODE_H

#include <stddef.h>
#include <stdio.h>

#include "command.h"
#include "options.h"
#include "tokenizer.h"

typedef struct _xargs_mode_ops xargs_mode_ops;

typedef struct _xargs_mode {
  xargs_mode_ops* ops;
  FILE* arg_source;
  command cmd;
  tokenizer* toker;
} xargs_mode;

struct _xargs_mode_ops {
  int (*run)(struct _xargs_mode* self);
};

void xargs_mode_init(
  xargs_mode* mode,
  xargs_mode_ops* ops,
  options* opts,
  int arg_index,
  int argc,
  char** argv);

int xargs_mode_run(xargs_mode* mode);

char* xargs_mode_next_token(xargs_mode* mode);

uint8_t xargs_mode_arg_would_exceed_limits(
  xargs_mode* mode,
  char* arg);

uint8_t xargs_mode_should_execute_command_after_arg_added(
  xargs_mode* mode);

int xargs_mode_execute_command(xargs_mode* mode);

void xargs_mode_add_input_argument(
  xargs_mode* mode,
  char* new_arg);

uint8_t xargs_mode_input_args_remain(xargs_mode* mode);

void xargs_mode_ensure_command_length_not_exceeded(
  xargs_mode* mode,
  char* new_arg);

void xargs_replace_args(xargs_mode* mode, char* new_arg);

void xargs_mode_destroy(xargs_mode* mode);

#endif  // PHXARGS_XARGS_MODE_H
