#include "xargs.h"

int main(int argc, char** argv) {
  xargs* x = xargs_create(argc, argv);
  int execution_status = xargs_run(x);
  xargs_destroy(x);

  return execution_status;
}
