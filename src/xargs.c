#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "options.h"
#include "tokenizer.h"
#include "util.h"
#include "xargs.h"
#include "xargs_mode.h"

struct xargs {
  xargs_mode mode;
};

xargs* xargs_init(int argc, char** argv) {
  options opts;
  init_options(&opts);
  int arg_index = parse_options(&opts, argc, argv);

  xargs* x = safe_malloc(sizeof(xargs));
  init_xargs_mode(&(x->mode), &opts, arg_index, argc, argv);
  return x;
}

int xargs_run(xargs* const x) {
  return run_xargs_in_mode(&(x->mode));
}

void xargs_free(xargs* const x) {
  free_xargs_mode(&(x->mode));
  free(x);
}
