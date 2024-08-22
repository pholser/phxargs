#include <assert.h>
#include <stdlib.h>

#include "command.h"
#include "delim_tokenizer.h"
#include "execution_context.h"
#include "options.h"
#include "space_tokenizer.h"
#include "util.h"

#define DEFAULT_CMD "/bin/echo"

void establish_context(execution_context* const ctx, int argc, char** argv) {
  options opts;
  init_options(&opts);
  int arg_index = parse_options(&opts, argc, argv);

  if (opts.use_nul_char_as_arg_delimiter || opts.arg_delimiter != '\0') {
    ctx->t_kind = DELIMITED;
    ctx->tokenizer = safe_malloc(sizeof(delim_tokenizer));
    init_delim_tokenizer(ctx->tokenizer, &opts);
  } else {
    ctx->t_kind = SPACE;
    ctx->tokenizer = safe_malloc(sizeof(space_tokenizer));
    init_space_tokenizer(ctx->tokenizer, &opts);
  }

  init_command(&(ctx->cmd), &opts);

  if (arg_index == argc) {
    add_fixed_argument(&(ctx->cmd), DEFAULT_CMD);
  } else {
    for (int i = arg_index; i < argc; ++i) {
      add_fixed_argument(&(ctx->cmd), argv[i]);
    }
  }
}

char* next_token(execution_context* const ctx) {
  switch (ctx->t_kind) {
    case DELIMITED:
      return next_delim_token((delim_tokenizer*) ctx->tokenizer);
    case SPACE:
      return next_space_token((space_tokenizer*) ctx->tokenizer, &(ctx->cmd));
    default:
      assert(!"reachable");
      exit(EXIT_FAILURE);
  }
}

int run_xargs(execution_context* const ctx) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  while ((token = next_token(ctx)) != NULL) {
    if (arg_would_exceed_limits(&(ctx->cmd), token)) {
      execution_status |= execute_command(&(ctx->cmd));
    }
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

void release_context(const execution_context* const ctx) {
  free_command(&(ctx->cmd));

  switch (ctx->t_kind) {
    case DELIMITED:
      free_delim_tokenizer((delim_tokenizer*) ctx->tokenizer);
      break;
    case SPACE:
      free_space_tokenizer((space_tokenizer*) ctx->tokenizer);
      break;
    default:
      assert(!"reachable");
      exit(EXIT_FAILURE);
  }
}
