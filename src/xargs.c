#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "options.h"
#include "tokenizer.h"
#include "util.h"
#include "xargs.h"
#include "xargs_mode.h"

void init_xargs(xargs* const x, int argc, char** argv) {
  options opts;
  init_options(&opts);

  int arg_index = parse_options(&opts, argc, argv);

  init_xargs_mode(&(x->mode), &opts, arg_index, argc, argv);
}

int run_xargs(xargs* const x) {
  return run_xargs_in_mode(&(x->mode));
}

void free_xargs(const xargs* const x) {
  free_xargs_mode(&(x->mode));
}
