#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "options.h"
#include "tokenizer.h"
#include "util.h"
#include "xargs.h"
#include "xargs_mode.h"

void xargs_init(xargs* const x, int argc, char** argv) {
  options opts;
  init_options(&opts);

  int arg_index = parse_options(&opts, argc, argv);

  init_xargs_mode(&(x->mode), &opts, arg_index, argc, argv);
}

int xargs_run(xargs* const x) {
  return run_xargs_in_mode(&(x->mode));
}

void xargs_free(const xargs* const x) {
  free_xargs_mode(&(x->mode));
}
