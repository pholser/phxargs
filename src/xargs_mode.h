#ifndef PHXARGS_XARGS_MODE_H
#define PHXARGS_XARGS_MODE_H

#include <stdbool.h>

#include "options.h"

typedef struct xargs_mode_s xargs_mode;
typedef struct xargs_mode_ops_s xargs_mode_ops;

struct xargs_mode_ops_s {
  int (*run)(xargs_mode* self);
  void (*destroy_impl)(void* impl);
};

xargs_mode* xargs_mode_create(
  const xargs_mode_ops* ops,
  const options* opts,
  int arg_index,
  int argc,
  const char* const* argv,
  void* impl);

void* xargs_mode_impl(const xargs_mode* mode);

int xargs_mode_run(xargs_mode* mode);

const char* xargs_mode_next_token(xargs_mode* mode);

bool xargs_mode_tokenizer_errored(const xargs_mode* mode);

bool xargs_mode_arg_would_exceed_limits(
  const xargs_mode* mode,
  const char* arg);

bool xargs_mode_should_execute_command_after_arg_added(xargs_mode* mode);

void xargs_mode_execute_command(xargs_mode* mode);

int xargs_mode_drain(xargs_mode* mode);

void xargs_mode_add_input_argument(xargs_mode* mode, const char* new_arg);

bool xargs_mode_input_args_remain(const xargs_mode* mode);

void xargs_mode_ensure_command_length_not_exceeded(
  const xargs_mode* mode,
  const char* new_arg);

void xargs_replace_args(xargs_mode* mode, const char* new_arg);

void xargs_mode_destroy(xargs_mode* mode);

#endif // PHXARGS_XARGS_MODE_H
