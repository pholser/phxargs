#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "arg_source.h"
#include "command.h"
#include "options.h"
#include "replacer_mode.h"
#include "tokenizer.h"
#include "util.h"

void init_replacer_mode(
  replacer_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  mode->arg_source = init_arg_source(opts);
  init_command(&(mode->cmd), opts, arg_index, argc, argv);
  init_tokenizer(&(mode->toker), opts, mode->cmd.max_length);
  mode->placeholder = safe_strdup(opts->arg_placeholder);
}

int run_replacer_mode(replacer_mode* const mode) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  for (token = next_token(&(mode->toker), mode->arg_source, &(mode->cmd));
    token != NULL;
    token = next_token(&(mode->toker), mode->arg_source, &(mode->cmd))) {

    // 1) retain cmd's original fixed args
    // 2) make replacement fixed args
    // 3) execute cmd
    // 4) recycle
    // 5) reinstall original fixed args
    // if (arg_would_exceed_limits(&(mode->cmd), token)) {
    //   execution_status |= execute_command(&(mode->cmd));
    // }
  }

  return execution_status;
}

void free_replacer_mode(const replacer_mode* const mode) {
  free(mode->placeholder);
  free_tokenizer(&(mode->toker));
  free_command(&(mode->cmd));
  free_arg_source(mode->arg_source);
}
