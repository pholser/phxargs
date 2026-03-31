#include "xargs.h"

int main(int argc, char** argv) {
  xargs* x = xargs_create(argc, (const char* const*) argv);
  int execution_status = xargs_run(x);
  xargs_destroy(x);

  return execution_status;
}
