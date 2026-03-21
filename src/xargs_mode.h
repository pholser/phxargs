#ifndef PHXARGS_XARGS_MODE_H
#define PHXARGS_XARGS_MODE_H

#include <stdint.h>

#include "options.h"

typedef struct _xargs_mode xargs_mode;
typedef struct _xargs_mode_ops xargs_mode_ops;

struct _xargs_mode_ops {
  int (*run)(xargs_mode* self);
  void (*destroy_impl)(void* impl);
};

xargs_mode* xargs_mode_create(
  xargs_mode_ops* ops,
  options* opts,
  int arg_index,
  int argc,
  char** argv,
  void* impl);

void* xargs_mode_impl(xargs_mode* mode);

int xargs_mode_run(xargs_mode* mode);

char* xargs_mode_next_token(xargs_mode* const mode);

uint8_t xargs_mode_arg_would_exceed_limits(
  xargs_mode* mode,
  char* arg);

uint8_t xargs_mode_should_execute_command_after_arg_added(
  xargs_mode* mode);

int xargs_mode_execute_command(xargs_mode* mode);

int xargs_mode_drain(xargs_mode* mode);

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
