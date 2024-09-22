#ifndef PHXARGS_XARGS_H
#define PHXARGS_XARGS_H

#include "xargs_mode.h"

typedef struct {
  xargs_mode mode;
} xargs;

void xargs_init(xargs* const x, int argc, char** argv);

int xargs_run(xargs* const x);

void xargs_free(const xargs* const x);

#endif  // PHXARGS_XARGS_H
