#include <stdio.h>
#include <stdlib.h>

#include "appender_mode.h"
#include "replacer_mode.h"
#include "options.h"
#include "util.h"
#include "xargs.h"
#include "xargs_mode.h"

struct _xargs {
  xargs_mode* mode;
};

xargs* xargs_create(int argc, char** argv) {
  options opts;
  init_options(&opts);
  int arg_index = parse_options(&opts, argc, argv);

  xargs* x = safe_malloc(sizeof(xargs));
  x->mode = opts.arg_placeholder == NULL
    ? (xargs_mode*) appender_mode_create(&opts, arg_index, argc, argv)
    : (xargs_mode*) replacer_mode_create(&opts, arg_index, argc, argv);
  return x;
}

int xargs_run(xargs* x) {
  return xargs_mode_run(x->mode);
}

void xargs_destroy(xargs* x) {
  xargs_mode_destroy(x->mode);
  free(x);
}
