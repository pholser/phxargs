#include "execution_context.h"

int main(int argc, char** argv) {
  execution_context ctx;

  establish_context(&ctx, argc, argv);
  int execution_status = run_xargs(&ctx);
  release_context(&ctx);

  return execution_status;
}
