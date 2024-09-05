#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "delim_tokenizer.h"
#include "execution_context.h"
#include "options.h"
#include "space_tokenizer.h"
#include "util.h"

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
    init_delim_tokenizer(
      ctx->tokenizer,
      ctx->cmd.max_length,
      opts->arg_delimiter);
  } else {
    ctx->t_kind = SPACE;
    ctx->tokenizer = safe_malloc(sizeof(space_tokenizer));
    init_space_tokenizer(
      ctx->tokenizer,
      ctx->cmd.max_length,
      options_line_mode(opts),
      opts->logical_end_of_input_marker);
  }
}

void establish_command(
  execution_context* const ctx,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  init_command(&(ctx->cmd), arg_index, argc, argv);
  config_command(&(ctx->cmd), opts);
}

void establish_context(execution_context* const ctx, int argc, char** argv) {
  options opts;
  init_options(&opts);

  int arg_index = parse_options(&opts, argc, argv);

  establish_arg_source(ctx, &opts);
  establish_command(ctx, &opts, arg_index, argc, argv);
  establish_tokenizer(ctx, &opts);
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

  for (char* token = next_token(ctx); token != NULL; token = next_token(ctx)) {
    if (arg_would_exceed_limits(&(ctx->cmd), token)) {
      execution_status |= execute_command(&(ctx->cmd));
    }

    add_input_argument(&(ctx->cmd), token);
    if (should_execute_command_after_arg_added(&(ctx->cmd))) {
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
