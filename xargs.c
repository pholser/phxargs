#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define INITIAL_ARGS_CAPACITY 10
#define DEFAULT_MAX_ARGS 5000

const char* option_flags = ":n:";

struct options {
  long max_args;
  char* max_args_endptr;
};

struct command_args {
  int count;
  int capacity;
  char** args;
};

struct tokenizing_buffer {
  char* buffer;
  size_t size;
  size_t processed;
};

void* safe_malloc(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    perror("malloc failed");
    exit(1);
  }
  return ptr;
}

void* safe_realloc(void *ptr, size_t size) {
  ptr = realloc(ptr, size);
  if (ptr == NULL) {
    perror("realloc failed");
    exit(1);
  }
  return ptr;
}

void handle_fork_error(pid_t pid) {
  if (pid < 0) {
    perror("fork failed");
    exit(1);
  }
}

void handle_execvp_error() {
  perror("execvp failed");
  exit(1);
}

void allocate_args(struct command_args* args) {
  args->count = 0;
  args->capacity = INITIAL_ARGS_CAPACITY;
  args->args = safe_malloc(args->capacity * sizeof(char*));
}

void free_arguments(struct command_args* args) {
  for (int i = 0; i < args->count; ++i) {
    free(args->args[i]);
  }
  free(args->args);
}

void reallocate_args_if_needed(struct command_args* args) {
  if (args->count >= args->capacity) {
    args->capacity *= 2;
    args->args = safe_realloc(args->args, args->capacity * sizeof(char*));
  }
}

void execute_command(
  struct options* opts,
  struct command_args* args,
  bool reallocate_args_after_execution) {

  pid_t pid = fork();
  handle_fork_error(pid);

  if (pid == 0) {
    // Child process
    args->args[args->count] = NULL;
    execvp("/bin/echo", args->args);
    handle_execvp_error();
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    if (status != 0) {
      perror("command failed");
      exit(1);
    }

    free_arguments(args);
    if (reallocate_args_after_execution) {
      allocate_args(args);
    }
  }
}

void add_argument(
  struct command_args* args,
  char* new_arg) {

  reallocate_args_if_needed(args);

  fprintf(stdout, "Adding arg %d: %s\n", args->count, new_arg);
  args->args[args->count] = strdup(new_arg);
  args->count++;
}

void process_chunk(
  struct tokenizing_buffer* buf,
  struct options* opts,
  struct command_args* args) {

  size_t token_start = buf->processed;
  int token_length = 0;

  for (size_t i = buf->processed; i < buf->size; ++i) {
    fprintf(stdout, "reading item %zu of buffer: %c\n", i, buf->buffer[i]);
    char ch = buf->buffer[i];
    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '\0') {
      if (token_start < i) {
        buf->buffer[i] = '\0';

        if (args->count == opts->max_args) {
	  fprintf(stdout, "execute command\n");
          fprintf(stdout, "arg count: %d\n", args->count);

	  execute_command(opts, args, true);
        }

        add_argument(args, buf->buffer + token_start);
      }

      token_start = i + 1;
    }
  }

  if (token_start < buf->size) {
    memmove(buf->buffer, buf->buffer + token_start, buf->size - token_start);
    buf->processed = 0;
    buf->size -= token_start;
  } else {
    buf->processed = 0;
    buf->size = 0;
  }
}

void run_xargs(
  struct options* opts,
  struct command_args* args) {

  struct tokenizing_buffer buf;
  memset(&buf, 0, sizeof(struct tokenizing_buffer));
  buf.buffer = safe_malloc(CHUNK_SIZE);

  while ((buf.size = fread(buf.buffer, 1, CHUNK_SIZE, stdin)) > 0) {
    process_chunk(&buf, opts, args);
  }
  if (ferror(stdin)) {
    fprintf(stderr, "xargs: I/O error\n");
    exit(1);
  }

  if (buf.size > 0) {
    buf.buffer[buf.size] = '\0';
    add_argument(args, buf.buffer);
  }

  if (args->count > 0) {
    fprintf(stdout, "execute last command\n");
    fprintf(stdout, "arg count: %d\n", args->count);
    execute_command(opts, args, false);
  }

  free(buf.buffer);
}

long parse_number_arg(int opt, const char* arg, char** endptr) {
  errno = 0;
  long parsed = strtol(arg, endptr, 10);

  if (arg == *endptr
    || errno != 0
    || (errno == 0 && **endptr != '\0')) {

    fprintf(stderr, "xargs: -%c %s: Invalid\n", opt, arg);
  }

  return parsed;
}

void parse_args(
  int argc,
  char** argv,
  struct options* opts,
  struct command_args* args) {

  int opt;
  while ((opt = getopt(argc, argv, option_flags)) != -1) {
    switch (opt) {
      case 'n':
        opts->max_args =
	  parse_number_arg(opt, optarg, &(opts->max_args_endptr));
        break;
      case ':':
        fprintf(stderr, "xargs: -%c needs an argument\n", optopt);
	exit(1);
        break;  
      case '?':  
        fprintf(stderr, "xargs: unknown option -%c\n", optopt);
	exit(1);
	break;
    }
  }
}

int main(int argc, char** argv) {
  struct options opts;
  memset(&opts, 0, sizeof(struct options));
  opts.max_args = DEFAULT_MAX_ARGS;

  struct command_args args;
  memset(&args, 0, sizeof(struct command_args));
  allocate_args(&args);

  parse_args(argc, argv, &opts, &args);
  run_xargs(&opts, &args);
  
  return 0;
}
