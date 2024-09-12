#ifndef PHXARGS_XARGS_H
#define PHXARGS_XARGS_H

#include "xargs_mode.h"

typedef struct {
  xargs_mode mode;
} xargs;

void init_xargs(xargs* const x, int argc, char** argv);

int run_xargs(xargs* const x);

void free_xargs(const xargs* const x);

#endif  // PHXARGS_XARGS_H
