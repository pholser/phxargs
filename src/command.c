#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "command.h"
#include "command_args.h"
#include "options.h"
#include "util.h"

pid_t safe_fork() {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  return pid;
}

void safe_exec(char** exec_args) {
  execvp(exec_args[0], exec_args);
  perror("execvp");
  exit(EXIT_FAILURE);
}

int command_status(pid_t child_pid) {
  int status;

  pid_t wait_result = waitpid(child_pid, &status, 0);

  if (wait_result == -1) {
    perror("waitpid failed");
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

void init_command(command* cmd, const options* opts) {
  cmd->fixed_args = allocate_args();
  cmd->input_args = allocate_args();

  cmd->max_lines = opts->max_lines_per_command;
  cmd->max_args = opts->max_args_per_command;
  cmd->max_length = opts->max_command_length;
  cmd->prompt = opts->prompt;
  cmd->trace = opts->trace;

  cmd->line_count = 0;
  cmd->length = 0;
}

void recycle_command(command* cmd) {
  cmd->line_count = 0;
  cmd->length = 0;
  free_args(cmd->input_args);
  cmd->input_args = allocate_args();
}

char** build_exec_args(command* cmd) {
  size_t exec_args_count = cmd->fixed_args->count + cmd->input_args->count;
  char** exec_args = safe_malloc((exec_args_count + 1) * sizeof(char*));
  for (size_t i = 0; i < cmd->fixed_args->count; ++i) {
    exec_args[i] = cmd->fixed_args->args[i];
  }
  for (size_t i = 0; i < cmd->input_args->count; ++i) {
    exec_args[cmd->fixed_args->count + i] = cmd->input_args->args[i];
  }
  exec_args[exec_args_count] = NULL;

  return exec_args;
}

bool should_execute_command(const command* cmd) {
  return cmd->input_args->count == cmd->max_args
    || cmd->line_count == cmd->max_lines
    || cmd->length >= cmd->max_length
    ;
}

bool confirm_execution() {
  FILE* tty = fopen("/dev/tty", "r");
  if (tty == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  char buf[4];
  if (fgets(buf, sizeof(buf), tty) == NULL) {
    perror("fgets");
    fclose(tty);
    exit(EXIT_FAILURE);
  }

  fclose(tty);
  return (buf[0] == 'y' || buf[0] == 'Y');
}

int execute_command(command* cmd) {
  pid_t pid = safe_fork();

  if (pid == 0) {
    // Child process
    size_t exec_args_count = cmd->fixed_args->count + cmd->input_args->count;
    char** exec_args = build_exec_args(cmd);

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

    bool execute = true;
    if (cmd->prompt) {
      fprintf(stderr, "?...");
      execute = confirm_execution();
    }

    if (execute) {
      safe_exec(exec_args);
    }
  } else {
    // Parent process
    int status = command_status(pid);

    recycle_command(cmd);
    return status;
  }

  recycle_command(cmd);
  return EXIT_SUCCESS;
}

void add_fixed_argument(command* cmd, const char* new_arg) {
  add_arg(cmd->fixed_args, new_arg);
  cmd->length += strlen(new_arg) + 1;
}

void add_input_argument(command* cmd, const char* new_arg) {
  add_arg(cmd->input_args, new_arg);
  cmd->length += strlen(new_arg) + 1;
}

void free_command(command* cmd) {
  free(cmd->fixed_args);
  free(cmd->input_args);
}
