#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "command.h"
#include "str.h"
#include "util.h"

#define DEFAULT_CMD "/bin/echo"

extern char** environ;

size_t env_length() {
  size_t sz = 0;
  for (char** e = environ; *e != NULL; ++e) {
    sz += strlen(*e) + 1;
  }
  return sz;
}

pid_t safe_fork() {
  pid_t pid = fork();
  if (pid < 0) {
    perror("phxargs: fork");
    exit(EXIT_FAILURE);
  }

  return pid;
}

void safe_exec(char** exec_args) {
  execvp(exec_args[0], exec_args);
  perror("phxargs: execvp");
  exit(EXIT_FAILURE);
}

int command_status(pid_t child_pid) {
  int status;

  pid_t wait_result = waitpid(child_pid, &status, 0);

  if (wait_result == -1) {
    perror("phxargs: waitpid");
    exit(EXIT_FAILURE);
  }
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    return WTERMSIG(status);
  }

  return EXIT_FAILURE;
}

void add_fixed_argument(command* const cmd, const char* const new_arg) {
  add_arg(&(cmd->fixed_args), new_arg);
}

void add_input_argument(command* const cmd, const char* const new_arg) {
  add_arg(&(cmd->input_args), new_arg);
}

size_t command_length(const command* const cmd) {
  return cmd->env_length
    + cmd->fixed_args.length
    + cmd->input_args.length;
}

uint8_t command_max_args_specified(const command* const cmd) {
  return cmd->max_args > 0;
}

size_t decide_max_length(command* const cmd, const options* const opts) {
  size_t max_length =
    ((size_t) sysconf(_SC_ARG_MAX)) - (2 * cmd->env_length) - 2048;
  if (options_max_command_length_specified(opts)) {
    size_t min_length = command_length(cmd);
    if (opts->max_command_length < min_length) {
      fprintf(
        stderr,
        "phxargs: -s %zu: too small (minimum %zu bytes)",
        opts->max_command_length,
        min_length);
      exit(EXIT_FAILURE);
    } else if (opts->max_command_length > max_length) {
      fprintf(
        stderr,
        "phxargs: -s %zu: too large (maximum %zu bytes)",
        opts->max_command_length,
        max_length);
      exit(EXIT_FAILURE);
    } else {
      return opts->max_command_length;
    }
  } else {
    return min(128 * 1024, max_length);
  }
}

void init_command(
  command* const cmd,
  const options* const opts,
  int arg_index,
  int argc,
  char** argv) {

  cmd->max_lines = opts->max_lines_per_command;
  cmd->max_args = opts->max_args_per_command;
  cmd->arg_placeholder = safe_strdup(opts->arg_placeholder);
  cmd->prompt = opts->prompt;
  cmd->trace = opts->trace;
  cmd->terminate_on_too_large_command = opts->terminate_on_too_large_command;
  cmd->line_mode = options_line_mode(opts);
  cmd->line_count = 0;
  cmd->env_length = env_length();

  if (arg_index == argc) {
    init_args_with_capacity(&(cmd->fixed_args), 1);
    add_fixed_argument(cmd, DEFAULT_CMD);
  } else {
    init_args_with_capacity(&(cmd->fixed_args), argc - arg_index);
    for (int i = arg_index; i < argc; ++i) {
      add_fixed_argument(cmd, argv[i]);
    }
  }

  if (command_max_args_specified(cmd)) {
    init_args_with_capacity(&(cmd->input_args), cmd->max_args);
  } else {
    init_args(&(cmd->input_args));
  }

  cmd->max_length = decide_max_length(cmd, opts);

  init_args_with_capacity(&(cmd->replaced_fixed_args), cmd->fixed_args.count);
}

void recycle_command(command* const cmd) {
  cmd->line_count = 0;

  free_args(&(cmd->input_args));

  if (cmd->arg_placeholder != NULL) {
    init_args_with_capacity(
      &(cmd->replaced_fixed_args),
      cmd->fixed_args.count);
  }

  if (command_max_args_specified(cmd)) {
    init_args_with_capacity(&(cmd->input_args), cmd->max_args);
  } else {
    init_args(&(cmd->input_args));
  }
}

void command_replace_args(command* const cmd, const char* const token) {
  // Do not perform replacement on command word
  add_arg(&(cmd->replaced_fixed_args), cmd->fixed_args.args[0]);

  for (size_t i = 1; i < cmd->fixed_args.count; ++i) {
    char* replaced =
      str_replace(cmd->fixed_args.args[i], cmd->arg_placeholder, token);
    add_arg(&(cmd->replaced_fixed_args), replaced);
    free(replaced);
  }
}

char** build_exec_args(command* cmd, size_t* exec_args_count) {
  command_args* fixed_args_in_play =
    cmd->arg_placeholder != NULL
      ? &(cmd->replaced_fixed_args)
      : &(cmd->fixed_args);

  *exec_args_count = fixed_args_in_play->count + cmd->input_args.count;

  char** exec_args = safe_calloc(*exec_args_count + 1, sizeof(char*));
  for (size_t i = 0; i < fixed_args_in_play->count; ++i) {
    exec_args[i] = safe_strdup(fixed_args_in_play->args[i]);
  }
  for (size_t i = 0; i < cmd->input_args.count; ++i) {
    exec_args[fixed_args_in_play->count + i] =
      safe_strdup(cmd->input_args.args[i]);
  }
  exec_args[*exec_args_count] = NULL;

  return exec_args;
}

  /*
   * As long as there are more arguments to read ...
   * (1) read the next arg, `x`
   * (2) after having read `x`:
   *   (a) adding `x` to an empty cmd may make command impossible
   *       due to overrunning. Fail it.
   *   (b) in max-args mode, we may be at max args before adding
   *       `x`. Execute cmd.
   *   (c) in line mode, we may have in the act of reading `x`
   *       incremented line count to the max. Execute cmd.
   *   (d) we may be at a point where adding `x` would cause us
   *       to exceed implied or explicit size bound. Execute cmd.
   * 
   * (3) Add `x` to cmd.
   */

uint8_t arg_would_exceed_limits(
  const command* const cmd,
  const char* const new_arg) {

  size_t new_length = command_length(cmd) + strlen(new_arg) + 1;

  if (cmd->input_args.count == 0) {
    if (new_length > cmd->max_length) {
      fprintf(stderr, "phxargs: command too long\n");
      exit(EXIT_FAILURE);
    }
  }

  if (command_max_args_specified(cmd)) {
    return cmd->input_args.count == cmd->max_args;
  }

  return cmd->line_mode ? 0 : new_length > cmd->max_length;
}

uint8_t should_execute_command_after_arg_added(const command* const cmd) {
  if (cmd->terminate_on_too_large_command
    && command_length(cmd) > cmd->max_length) {

    fprintf(stderr, "phxargs: command too long\n");
    exit(EXIT_FAILURE);
  }

  if (cmd->line_mode) {
    return cmd->line_count == cmd->max_lines;
  }

  return 0;
}

uint8_t confirm_execution() {
  FILE* tty = fopen("/dev/tty", "r");
  if (tty == NULL) {
    perror("phxargs: cannot open /dev/tty");
    exit(EXIT_FAILURE);
  }

  char buf[4];
  if (fgets(buf, sizeof(buf), tty) == NULL) {
    perror("phxargs: cannot read from /dev/tty");
    fclose(tty);
    exit(EXIT_FAILURE);
  }

  fclose(tty);
  return (buf[0] == 'y' || buf[0] == 'Y');
}

int execute_command(command* const cmd) {
  pid_t pid = safe_fork();

  if (pid == 0) {
    // Child process
    size_t exec_args_count = 0;
    char** exec_args = build_exec_args(cmd, &exec_args_count);

    if (cmd->trace) {
      for (size_t i = 0; i < exec_args_count; ++i) {
        fprintf(
          stderr,
          "%s%s",
          exec_args[i],
          (i < exec_args_count - 1) ? " " : "");
      }
      if (!cmd->prompt) {
        fprintf(stderr, "\n");
      }
    }

    uint8_t execute = 1;
    if (cmd->prompt) {
      fprintf(stderr, "?...");
      execute = confirm_execution();
    }

    if (execute) {
      safe_exec(exec_args);
    } else {
      for (size_t i = 0; i < exec_args_count; ++i) {
        free(exec_args[i]);
      }
      free(exec_args);
    }

    return EXIT_SUCCESS;
  } else {
    // Parent process
    int status = command_status(pid);

    recycle_command(cmd);
    return status;
  }
}

void free_command(const command* const cmd) {
  free_args(&(cmd->input_args));
  free_args(&(cmd->fixed_args));
  free(cmd->arg_placeholder);
}
