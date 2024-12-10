#ifndef PHXARGS_XARGS_H
#define PHXARGS_XARGS_H

typedef struct _xargs xargs;

xargs* xargs_create(int argc, char** argv);

int xargs_run(xargs* x);

void xargs_destroy(xargs* x);

#endif  // PHXARGS_XARGS_H
