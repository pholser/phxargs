#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHUNK_SIZE 8192
#define INITIAL_ARGS_CAPACITY 10
#define DEFAULT_MAX_ARGS_PER_COMMAND 5000
#define DEFAULT_MAX_COMMAND_LENGTH -1

const char* default_command = "/bin/echo";

typedef struct {
  /* -L option */
  unsigned long max_lines_per_command;
  char* max_lines_endptr;

  /* -n option */
  unsigned long max_args_per_command;
  char* max_args_endptr;

  /* -p option */
  bool prompt;

  /* -s option */
  unsigned long max_command_length;
  char* max_command_length_endptr;

  /* -t option */
  bool trace;
} options;

typedef struct {
  size_t count;
  size_t capacity;
  char** args;
} command_args;

typedef struct {
  size_t line_count;
  size_t length;
  command_args fixed_args;
  command_args input_args;
} command;

typedef struct {
  char* buffer;
  size_t size;
  size_t processed;
} tokenizing_buffer;

typedef struct {
  bool in_quote;
  char quote_char;
  bool escape;
  size_t token_start;
} parser_state;

void* safe_malloc(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
  ptr = realloc(ptr, size);
  if (ptr == NULL) {
    perror("realloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void handle_fork_error(pid_t pid) {
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

void handle_execvp_error() {
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

void allocate_args(command_args* args) {
  args->count = 0;
  args->capacity = INITIAL_ARGS_CAPACITY;
  args->args = safe_malloc(args->capacity * sizeof(char*));
}

void free_args(const command_args* args) {
  for (size_t i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }
  free(args->args);
}

void reallocate_args_if_needed(command_args* args) {
  if (args->count >= args->capacity) {
    args->capacity *= 2;
    args->args =
      safe_realloc(args->args, args->capacity * sizeof(char*));
  }
}

void init_command(command* cmd) {
  cmd->line_count = 0;
  cmd->length = 0;
  allocate_args(&cmd->fixed_args);
  allocate_args(&cmd->input_args);
}

void recycle_command(command* cmd) {
  cmd->line_count = 0;
  cmd->length = 0;
  free_args(&cmd->input_args);
  allocate_args(&cmd->input_args);
}

void free_command(command* cmd) {
  free_args(&cmd->fixed_args);
  free_args(&cmd->input_args);
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

bool should_execute_command(const command* cmd, const options* opts) {
//  fprintf(stderr, "max command length: %zu\n", opts->max_command_length);
//  fprintf(stderr, "max args per command: %zu\n", opts->max_args_per_command);
//  fprintf(stderr, "max lines per command: %zu\n", opts->max_lines_per_command);
//
//  fprintf(stderr, "most recent arg: %s\n", cmd->input_args.args[cmd->input_args.count - 1]);
//  fprintf(stderr, "command length: %zu\n", cmd->length);
//  fprintf(stderr, "line count: %zu\n", cmd->line_count);
//
  return cmd->input_args.count == opts->max_args_per_command
    || (opts->max_lines_endptr != NULL
        && cmd->line_count == opts->max_lines_per_command)
    || cmd->length >= opts->max_command_length
  ;
}

int execute_command(const options* opts, command* cmd) {
  pid_t pid = fork();
  handle_fork_error(pid);

  if (pid == 0) {
    // Child process
    size_t exec_args_count = cmd->fixed_args.count + cmd->input_args.count;
    char** exec_args = safe_malloc((exec_args_count + 1) * sizeof(char*));
    for (size_t i = 0; i < cmd->fixed_args.count; ++i) {
      exec_args[i] = cmd->fixed_args.args[i];
    }
    for (size_t i = 0; i < cmd->input_args.count; ++i) {
      exec_args[cmd->fixed_args.count + i] = cmd->input_args.args[i];
    }
    exec_args[exec_args_count] = NULL;

    if (opts->trace) {
      for (size_t i = 0; i < exec_args_count; ++i) {
        fprintf(
          stderr,
          "%s%s",
          exec_args[i],
          (i < exec_args_count - 1) ? " " : "");
      }
      if (!opts->prompt) {
        fprintf(stderr, "\n");
      }
    }

    bool execute = true;
    if (opts->prompt) {
      fprintf(stderr, "?...");
      execute = confirm_execution();
    }

    if (execute) {
      execvp(exec_args[0], exec_args);
      handle_execvp_error();
      return EXIT_FAILURE;
    }
  } else {
    // Parent process
    int status = command_status(pid);
    waitpid(pid, &status, 0);

    recycle_command(cmd);
    return status;
  }

  recycle_command(cmd);
  return EXIT_SUCCESS;
}

void add_argument(command_args* args, const char* new_arg) {
  reallocate_args_if_needed(args);

  args->args[args->count] = strdup(new_arg);
  args->count++;
}

void add_fixed_argument(command* cmd, const char* new_arg) {
  add_argument(&(cmd->fixed_args), new_arg);
  cmd->length += strlen(new_arg) + 1;
}

void add_input_argument(command* cmd, const char* new_arg) {
  add_argument(&(cmd->input_args), new_arg);
  cmd->length += strlen(new_arg) + 1;
}

void process_chunk(
  tokenizing_buffer* buf,
  const options* opts,
  command* cmd,
  parser_state* pstate,
  int* execution_status) {

  for (size_t i = buf->processed; i < buf->size; ++i) {
    char ch = buf->buffer[i];

    if (pstate->escape) {
      pstate->escape = false;
      continue;
    }
    if (ch == '\\') {
      pstate->escape = true;
      continue;
    }
    if (pstate->in_quote) {
      if (ch == pstate->quote_char) {
        pstate->in_quote = false;
      }
      continue;
    }
    if (ch == '\'' || ch == '"') {
      pstate->in_quote = true;
      pstate->quote_char = ch;
      continue;
    }

    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '\0') {
      if (ch == '\n') {
        cmd->line_count++;
      }

      if (pstate->token_start < i) {
        buf->buffer[i] = '\0';
        add_input_argument(cmd, buf->buffer + pstate->token_start);

        if (should_execute_command(cmd, opts)) {
          *execution_status |= execute_command(opts, cmd);
        }
      }

      pstate->token_start = i + 1;
    }
  }

  if (pstate->token_start < buf->size) {
    size_t leftover_size = buf->size - pstate->token_start;
    memmove(buf->buffer, buf->buffer + pstate->token_start, leftover_size);
    buf->processed = leftover_size;
  } else {
    buf->processed = 0;
  }

  pstate->token_start = 0;
}

int run_xargs(const options* opts, command* cmd) {
  tokenizing_buffer buf = {0};
  buf.buffer = safe_malloc(CHUNK_SIZE + 1);

  parser_state pstate = {0};

  int execution_status = EXIT_SUCCESS;

  while ((buf.size = fread(
    buf.buffer + buf.processed,
    1,
    CHUNK_SIZE - buf.processed,
    stdin)) > 0) {

    buf.size += buf.processed;
    buf.buffer[buf.size] = '\0';
    process_chunk(&buf, opts, cmd, &pstate, &execution_status);
  }
  if (ferror(stdin)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  }

  if (buf.size > 0) {
    buf.buffer[buf.size] = '\0';
    add_input_argument(cmd, buf.buffer);
  }

  if (cmd->input_args.count > 0) {
    execution_status |= execute_command(opts, cmd);
  }

  free(buf.buffer);
  return execution_status;
}

long parse_number_arg(int opt, const char* arg, char** endptr) {
  errno = 0;
  long parsed = strtol(arg, endptr, 10);

  if (arg == *endptr
      || errno != 0
      || (errno == 0 && **endptr != '\0')) {

    fprintf(stderr, "phxargs: -%c %s: Invalid\n", opt, arg);
  }

  return parsed;
}

void parse_args(int argc, char** argv, options* opts, command* cmd) {
  opts->max_args_per_command = DEFAULT_MAX_ARGS_PER_COMMAND;
  opts->max_command_length = DEFAULT_MAX_COMMAND_LENGTH;

  long num;
  int opt;

  while ((opt = getopt(argc, argv, ":L:n:ps:t")) != -1) {
    switch (opt) {
      case 'L':
        num = parse_number_arg(opt, optarg, &(opts->max_lines_endptr));
        opts->max_lines_per_command = (num > 0) ? num : -1;
        break;
      case 'n':
        opts->max_args_per_command =
          parse_number_arg(opt, optarg, &(opts->max_args_endptr));
        break;
      case 'p':
        opts->prompt = true;
        opts->trace = true;
        break;
      case 's':
        num =
          parse_number_arg(opt, optarg, &(opts->max_command_length_endptr));
        opts->max_command_length =
          (num > 0) ? num : DEFAULT_MAX_COMMAND_LENGTH;
        break;
      case 't':
        opts->trace = true;
        break;
      case ':':
        fprintf(stderr, "phxargs: -%c needs an argument\n", optopt);
        exit(EXIT_FAILURE);
      case '?':
        fprintf(stderr, "phxargs: unknown option -%c\n", optopt);
        exit(EXIT_FAILURE);
    }
  }

  if (optind == argc) {
    add_fixed_argument(cmd, default_command);
  } else {
    for (int i = optind; i < argc; ++i) {
      add_fixed_argument(cmd, argv[i]);
    }
  }
}

int main(int argc, char** argv) {
  options opts = {0};
  command cmd = {0};

  init_command(&cmd);
  parse_args(argc, argv, &opts, &cmd);
  int execution_status = run_xargs(&opts, &cmd);
  free_command(&cmd);

  return execution_status;
}
