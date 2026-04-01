#include "xargs.h"

#include <stdlib.h>

#include "appender_mode.h"
#include "options.h"
#include "process_pool.h"
#include "replacer_mode.h"
#include "util.h"
#include "xargs_mode.h"

struct xargs_s {
  xargs_mode* mode;
};

xargs* xargs_create(int argc, const char* const* argv) {
  process_pool_install_signal_handlers();
  options* opts = options_create(argc, argv);
  int arg_index = options_index_of_first_arg(opts);

  xargs* x = phxargs_malloc(sizeof(xargs));

  x->mode =
    options_arg_placeholder(opts) == NULL
      ? appender_mode_as_xargs_mode(
          appender_mode_create(opts, arg_index, argc, argv))
      : replacer_mode_as_xargs_mode(
          replacer_mode_create(opts, arg_index, argc, argv));

  options_destroy(opts);

  return x;
}

int xargs_run(xargs* x) {
  return xargs_mode_run(x->mode);
}

void xargs_destroy(xargs* x) {
  xargs_mode_destroy(x->mode);
  free(x);
}
