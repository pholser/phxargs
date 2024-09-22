#include "xargs.h"

int main(int argc, char** argv) {
  xargs x;

  xargs_init(&x, argc, argv);
  int execution_status = xargs_run(&x);
  xargs_free(&x);

  return execution_status;
}
