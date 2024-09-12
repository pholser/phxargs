#include <stdlib.h>

#include "appender_mode.h"
#include "arg_source.h"
#include "command.h"
#include "options.h"
#include "tokenizer.h"

void init_appender_mode(
  appender_mode* const mode,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  mode->arg_source = init_arg_source(opts);
  init_command(&(mode->cmd), opts, arg_index, argc, argv);
  init_tokenizer(&(mode->toker), opts, mode->cmd.max_length);
}

int run_appender_mode(appender_mode* const mode) {
  int execution_status = EXIT_SUCCESS;

  char* token;
  for (token = next_token(&(mode->toker), mode->arg_source, &(mode->cmd));
    token != NULL;
    token = next_token(&(mode->toker), mode->arg_source, &(mode->cmd))) {

    if (arg_would_exceed_limits(&(mode->cmd), token)) {
      execution_status |= execute_command(&(mode->cmd));
    }

    add_input_argument(&(mode->cmd), token);
    if (should_execute_command_after_arg_added(&(mode->cmd))) {
      execution_status |= execute_command(&(mode->cmd));
    }
  }

  if (mode->cmd.input_args.count > 0) {
    execution_status |= execute_command(&(mode->cmd));
  }

  return execution_status;
}

void free_appender_mode(const appender_mode* const mode) {
  free_arg_source(mode->arg_source);
  free_command(&(mode->cmd));
  free_tokenizer(&(mode->toker));
}
