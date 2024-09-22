#ifndef PHXARGS_XARGS_H
#define PHXARGS_XARGS_H

#include "xargs_mode.h"

typedef struct xargs xargs;

xargs* xargs_init(int argc, char** argv);

int xargs_run(xargs* const x);

void xargs_free(xargs* const x);

#endif  // PHXARGS_XARGS_H
