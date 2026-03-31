#include "command.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "command_args.h"
#include "exit_codes.h"
#include "str.h"
#include "util.h"

#define DEFAULT_CMD "/bin/echo"

extern char** environ;

struct command_s {
  size_t max_lines;
  size_t max_args;
  size_t max_length;
  char* arg_placeholder;
  uint8_t open_tty;
  uint8_t prompt;
  uint8_t trace;
  uint8_t terminate_on_too_large_command;
  uint8_t line_mode;

  size_t line_count;
  size_t env_length;
  command_args* fixed_args;
  command_args* input_args;
  command_args* replaced_fixed_args;
};

static size_t calc_env_length(void) {
  size_t sz = 0;
  for (char** e = environ; *e != NULL; ++e) {
    sz += strlen(*e) + 1;
  }
  return sz;
}

static pid_t safe_fork(void) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("phxargs: fork");
    exit(EXIT_FAILURE);
  }

  return pid;
}

static void safe_exec(char** exec_args, bool open_tty) {
  if (open_tty) {
    if (freopen("/dev/tty", "r", stdin) == NULL) {
      perror("phxargs: cannot reopen stdin as /dev/tty");
      exit(EXIT_FAILURE);
    }
  }

  execvp(exec_args[0], exec_args);

  int failed_result = errno;
  if (failed_result == E2BIG) {
    fprintf(stderr, "phxargs: command line too long -- this is a bug\n");
    exit(PHXARGS_STATUS_INTERNAL_ERROR);
  }

  fprintf(stderr, "phxargs: %s: %s\n", exec_args[0], strerror(failed_result));
  exit(
    failed_result == ENOENT
      ? PHXARGS_STATUS_NOT_FOUND
      : PHXARGS_STATUS_NOT_EXECUTABLE);
}

static void add_fixed_argument(command* cmd, const char* new_arg) {
  command_args_add(cmd->fixed_args, new_arg);
}

static bool command_max_args_specified(const command* cmd) {
  return cmd->max_args > 0;
}

static size_t decide_max_length(const command* cmd, const options* opts) {
  long sc_arg_max = safe_sysconf(_SC_ARG_MAX);

  /* Subtract 2x env_length as a conservative hedge: some platforms count
   * environment variables twice in their ARG_MAX accounting (once for the
   * string data, once for the pointer array). The extra 2048 bytes covers
   * the argv pointer array overhead and other kernel bookkeeping. */
  size_t max_length =
    sc_arg_max > 0
      ? (size_t) sc_arg_max - (2 * cmd->env_length) - 2048
      : (size_t) 128 * 1024;

  if (!options_max_command_length_specified(opts)) {
    return min((size_t) 128 * 1024, max_length);
  }

  size_t min_length = command_length(cmd);
  size_t specified = options_max_command_length(opts);

  if (specified < min_length) {
    fprintf(
      stderr,
      "phxargs: -s %zu: too small (minimum %zu bytes)\n",
      specified,
      min_length);
    exit(EXIT_FAILURE);
  }

  if (specified > max_length) {
    fprintf(
      stderr,
      "phxargs: -s %zu: too large (maximum %zu bytes)\n",
      specified,
      max_length);
    exit(EXIT_FAILURE);
  }

  return specified;
}

static void recycle_command(command* cmd) {
  cmd->line_count = 0;

  command_args_destroy(cmd->input_args);

  if (cmd->arg_placeholder != NULL) {
    command_args_destroy(cmd->replaced_fixed_args);
    cmd->replaced_fixed_args =
      command_args_create_with_capacity(command_args_count(cmd->fixed_args));
  }

  if (command_max_args_specified(cmd)) {
    cmd->input_args = command_args_create_with_capacity(cmd->max_args);
  } else {
    cmd->input_args = command_args_create();
  }
}

static char** build_exec_args(command* cmd, size_t* exec_args_count) {
  command_args* fixed_args_in_play =
    cmd->arg_placeholder != NULL ? cmd->replaced_fixed_args : cmd->fixed_args;

  *exec_args_count = command_args_count(fixed_args_in_play)
    + command_args_count(cmd->input_args);

  char** exec_args = (char**) safe_calloc(*exec_args_count + 1, sizeof(char*));
  for (size_t i = 0; i < command_args_count(fixed_args_in_play); ++i) {
    exec_args[i] = safe_strdup(command_args_at(fixed_args_in_play, i));
  }
  for (size_t i = 0; i < command_args_count(cmd->input_args); ++i) {
    exec_args[command_args_count(fixed_args_in_play) + i] =
      safe_strdup(command_args_at(cmd->input_args, i));
  }
  exec_args[*exec_args_count] = NULL;

  return exec_args;
}

static bool confirm_execution(void) {
  FILE* tty = fopen("/dev/tty", "r");
  if (tty == NULL) {
    perror("phxargs: cannot open /dev/tty");
    exit(EXIT_FAILURE);
  }

  int first = fgetc(tty);

  int ch;
  while ((ch = fgetc(tty)) != '\n' && ch != EOF) {
  }

  fclose(tty);
  return (first == 'y' || first == 'Y');
}

command* command_create(options* opts, int arg_index, int argc, char** argv) {
  command* cmd = safe_malloc(sizeof(command));

  cmd->max_lines = options_max_lines_per_command(opts);
  cmd->max_args = options_max_args_per_command(opts);
  cmd->arg_placeholder = safe_strdup(options_arg_placeholder(opts));
  cmd->open_tty = options_open_tty(opts);
  cmd->prompt = options_prompt(opts);
  cmd->trace = options_trace(opts);
  cmd->terminate_on_too_large_command =
    options_terminate_on_too_large_command(opts);
  cmd->line_mode = options_line_mode(opts);
  cmd->line_count = 0;
  cmd->env_length = calc_env_length();

  if (arg_index == argc) {
    cmd->fixed_args = command_args_create_with_capacity(1);
    add_fixed_argument(cmd, DEFAULT_CMD);
  } else {
    cmd->fixed_args = command_args_create_with_capacity(argc - arg_index);
    for (int i = arg_index; i < argc; ++i) {
      add_fixed_argument(cmd, argv[i]);
    }
  }

  if (command_max_args_specified(cmd)) {
    cmd->input_args = command_args_create_with_capacity(cmd->max_args);
  } else {
    cmd->input_args = command_args_create();
  }

  cmd->max_length = decide_max_length(cmd, opts);

  cmd->replaced_fixed_args =
    command_args_create_with_capacity(command_args_count(cmd->fixed_args));

  return cmd;
}

bool command_arg_would_exceed_limits(const command* cmd, const char* new_arg) {
  command_ensure_length_not_exceeded(cmd, new_arg);

  size_t new_length = command_length(cmd) + strlen(new_arg) + 1;
  bool would_exceed_size = !cmd->line_mode && new_length > cmd->max_length;

  if (command_max_args_specified(cmd)) {
    bool at_max_args = command_args_count(cmd->input_args) == cmd->max_args;
    if (cmd->terminate_on_too_large_command) {
      return at_max_args;
    }
    return at_max_args || would_exceed_size;
  }

  return would_exceed_size;
}

bool command_should_execute_after_arg_added(const command* cmd) {
  if (cmd->terminate_on_too_large_command
    && command_length(cmd) > cmd->max_length) {

    fprintf(stderr, "phxargs: command too long\n");
    exit(EXIT_FAILURE);
  }

  if (cmd->line_mode) {
    return cmd->line_count == cmd->max_lines;
  }

  return false;
}

void command_replace_args(command* cmd, const char* new_arg) {
  // Do not perform replacement on command word
  command_args_add(
    cmd->replaced_fixed_args, command_args_at(cmd->fixed_args, 0));

  for (size_t i = 1; i < command_args_count(cmd->fixed_args); ++i) {
    char* replaced =
      str_replace(
        command_args_at(cmd->fixed_args, i),
        cmd->arg_placeholder, new_arg);
    command_args_add(cmd->replaced_fixed_args, replaced);
    free(replaced);
  }
}

void command_ensure_length_not_exceeded(
  const command* cmd,
  const char* new_arg) {

  if (cmd->arg_placeholder != NULL
    || command_args_count(cmd->input_args) == 0) {

    size_t new_length = command_length(cmd) + strlen(new_arg) + 1;
    if (new_length > cmd->max_length) {
      fprintf(stderr, "phxargs: command too long\n");
      exit(EXIT_FAILURE);
    }
  }
}

pid_t command_execute_async(command* cmd) {
  pid_t pid = safe_fork();

  // Parent process
  if (pid > 0) {
    recycle_command(cmd);
    return pid;
  }

  // Child process
  size_t exec_args_count = 0;
  char** exec_args = build_exec_args(cmd, &exec_args_count);

  if (cmd->trace) {
    for (size_t i = 0; i < exec_args_count; ++i) {
      fprintf(stderr, "%s", exec_args[i]);

      if (i < exec_args_count - 1) {
        fprintf(stderr, " ");
      }
    }
    if (!cmd->prompt) {
      fprintf(stderr, "\n");
    }
  }

  bool execute = true;
  if (cmd->prompt) {
    fprintf(stderr, "?...");
    execute = confirm_execution();
  }

  if (execute) {
    safe_exec(exec_args, cmd->open_tty);
  } else {
    for (size_t i = 0; i < exec_args_count; ++i) {
      free(exec_args[i]);
    }
    free((void*) exec_args);
  }

  exit(EXIT_SUCCESS);
}

size_t command_max_length(const command* cmd) {
  return cmd->max_length;
}

void command_add_input_argument(command* cmd, const char* new_arg) {
  command_args_add(cmd->input_args, new_arg);
}

bool command_input_args_remain(const command* cmd) {
  return command_args_count(cmd->input_args) > 0;
}

size_t command_length(const command* cmd) {
  return cmd->env_length + command_args_length(cmd->fixed_args)
    + command_args_length(cmd->input_args);
}

void command_increment_line_count(command* cmd) {
  ++cmd->line_count;
}

void command_destroy(command* cmd) {
  command_args_destroy(cmd->input_args);
  command_args_destroy(cmd->fixed_args);
  command_args_destroy(cmd->replaced_fixed_args);
  free(cmd->arg_placeholder);
  free(cmd);
}
