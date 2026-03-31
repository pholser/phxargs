#ifndef PHXARGS_XARGS_H
#define PHXARGS_XARGS_H

typedef struct xargs_s xargs;

xargs* xargs_create(int argc, const char* const* argv);

int xargs_run(xargs* x);

void xargs_destroy(xargs* x);

#endif // PHXARGS_XARGS_H
