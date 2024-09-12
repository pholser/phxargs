#include "xargs.h"

int main(int argc, char** argv) {
  xargs x;

  init_xargs(&x, argc, argv);
  int execution_status = run_xargs(&x);
  free_xargs(&x);

  return execution_status;
}
