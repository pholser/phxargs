#include <stdlib.h>

#include "command.h"
#include "execution_context.h"
#include "options.h"
#include "util.h"

#define DEFAULT_CMD "/bin/echo"

void establish_context(execution_context* ctx, int argc, char** argv) {
  options opts;
  init_options(&opts);

  int arg_index = parse_options(&opts, argc, argv);
  init_tokenizer(&(ctx->tokenizer), &opts);
  init_command(&(ctx->cmd), &opts);

  if (arg_index == argc) {
    add_fixed_argument(&(ctx->cmd), DEFAULT_CMD);
  } else {
    for (int i = arg_index; i < argc; ++i) {
      add_fixed_argument(&(ctx->cmd), argv[i]);
    }
  }
}

int run_xargs(execution_context* ctx) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  while ((token = next_token(&(ctx->tokenizer))) != NULL) {
    add_input_argument(&(ctx->cmd), token);

    if (should_execute_command(&(ctx->cmd))) {
      execution_status |= execute_command(&(ctx->cmd));
    }
  }

  if (ctx->cmd.input_args->count > 0) {
    execution_status |= execute_command(&(ctx->cmd));
  }

  return execution_status;
}

void release_context(execution_context* ctx) {
  free_command(&(ctx->cmd));
  free_tokenizer(&(ctx->tokenizer));
}
