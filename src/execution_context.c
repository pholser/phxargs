#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "delim_tokenizer.h"
#include "execution_context.h"
#include "options.h"
#include "space_tokenizer.h"
#include "util.h"

#define DEFAULT_CMD "/bin/echo"

void establish_arg_source(
  execution_context* const ctx,
  const options* const opts) {

  ctx->arg_source = stdin;
  if (opts->arg_file_path != NULL) {
    FILE* arg_file = fopen(opts->arg_file_path, "r");
    if (arg_file == NULL) {
      perror("phxargs: cannot open arg file");
      exit(EXIT_FAILURE);
    } else {
      ctx->arg_source = arg_file;
    }
  }
}

void establish_tokenizer(
  execution_context* const ctx,
  const options* const opts) {

  if (opts->use_nul_char_as_arg_delimiter || opts->arg_delimiter != '\0') {
    ctx->t_kind = DELIMITED;
    ctx->tokenizer = safe_malloc(sizeof(delim_tokenizer));
    init_delim_tokenizer(ctx->tokenizer, opts);
  } else {
    ctx->t_kind = SPACE;
    ctx->tokenizer = safe_malloc(sizeof(space_tokenizer));
    init_space_tokenizer(ctx->tokenizer, opts);
  }
}

void establish_command(
  execution_context* const ctx,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  init_command(&(ctx->cmd), opts);

  if (arg_index == argc) {
    add_fixed_argument(&(ctx->cmd), DEFAULT_CMD);
  } else {
    for (int i = arg_index; i < argc; ++i) {
      add_fixed_argument(&(ctx->cmd), argv[i]);
    }
  }
}

void establish_context(execution_context* const ctx, int argc, char** argv) {
  options opts;
  init_options(&opts);

  int arg_index = parse_options(&opts, argc, argv);

  establish_arg_source(ctx, &opts);
  establish_tokenizer(ctx, &opts);
  establish_command(ctx, &opts, arg_index, argc, argv);
}

char* next_token(execution_context* const ctx) {
  switch (ctx->t_kind) {
    case DELIMITED:
      return next_delim_token(
        (delim_tokenizer*) ctx->tokenizer,
        ctx->arg_source);
    case SPACE:
      return next_space_token(
        (space_tokenizer*) ctx->tokenizer,
        ctx->arg_source,
        &(ctx->cmd));
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
  }

  if (ctx->arg_source != stdin) {
    fclose(ctx->arg_source);
  }
}
