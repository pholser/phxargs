#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define INITIAL_ARGS_CAPACITY 10

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
};

struct tokenizing_leftovers {
  char* leftover;
  size_t size;
};


void execute_command(
  struct options* opts,
  struct command_args* args,
  bool reallocate_args_after_execution) {

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    args->args[args->count] = NULL;
    if (execvp("/bin/echo", args->args) == -1) {
      perror("execvp failed");
      exit(1);
    }
  } else if (pid > 0) {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    if (status != 0) {
      perror("command failed");
      exit(1);
    }

    for (int i = 0; i < args->count; ++i) {
      free(args->args[i]);
    }
    free(args->args);

    if (reallocate_args_after_execution) {
      args->count = 0;
      args->capacity = INITIAL_ARGS_CAPACITY;
      args->args = malloc(args->capacity * sizeof(char*));
      if (args->args == NULL) {
        perror("malloc failed");
        exit(1);
      }
    }
  } else {
    perror("fork failed");
    exit(1);
  }
}

void add_argument(
  struct command_args* args,
  char* new_arg) {

  if (args->count >= args->capacity) {
    args->capacity *= 2;
    args->args = realloc(args->args, args->capacity * sizeof(char*));
    if (args->args == NULL) {
       perror("realloc failed");
       exit(1);
    }
  }

  fprintf(stdout, "Adding arg %d: %s\n", args->count, new_arg);
  args->args[args->count] = strdup(new_arg);
  args->count++;
}

void process_chunk(
  struct tokenizing_buffer* buf,
  struct options* opts,
  struct command_args* args,
  struct tokenizing_leftovers* leftover) {

  size_t token_start = 0;
  int token_length = 0;

  if (leftover->size > 0) {
    // Combine leftover with the current chunk
    size_t total_size = leftover->size + buf->size;

    char* combined = malloc(total_size);
    if (combined == NULL) {
      perror("malloc failed");
      exit(1);
    }
    memcpy(combined, leftover->leftover, leftover->size);
    memcpy(combined + leftover->size, buf->buffer, buf->size);
    free(buf->buffer);

    buf->buffer = combined;
    buf->size = total_size;
    leftover->size = 0;
  }

  for (size_t i = 0; i < buf->size; ++i) {
    fprintf(stdout, "reading item %zu of buffer: %c\n", i, buf->buffer[i]);
    char ch = buf->buffer[i];
    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '\0') {
      if (token_start < i) {
        buf->buffer[i] = '\0';
        token_length = i - token_start;
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
    leftover->size = buf->size - token_start;
    leftover->leftover = malloc(leftover->size);
    if (leftover->leftover == NULL) {
      perror("malloc failed");
      exit(1);
    }

    memcpy(
      leftover->leftover,
      buf->buffer + token_start,
      leftover->size);
    fprintf(stdout, "Leftover is now: %s %zu\n", leftover->leftover, leftover->size);
  }
}


void run_xargs(struct options* opts, struct command_args* args, char** envp) {
  struct tokenizing_buffer buf;
  memset(&buf, 0, sizeof(struct tokenizing_buffer));
  buf.buffer = malloc(CHUNK_SIZE);
  if (buf.buffer == NULL) {
    perror("malloc failed");
    exit(1);
  }

  struct tokenizing_leftovers leftover;
  memset(&leftover, 0, sizeof(struct tokenizing_leftovers));

  while ((buf.size = fread(buf.buffer, 1, CHUNK_SIZE, stdin)) > 0) {
    process_chunk(&buf, opts, args, &leftover);
  }
  if (ferror(stdin)) {
    fprintf(stderr, "xargs: I/O error\n");
    exit(1);
  }

  free(buf.buffer);

  if (leftover.size > 0) {
    leftover.leftover[leftover.size] = '\0';
    add_argument(args, leftover.leftover);
    free(leftover.leftover);
  }

  if (args->count > 0) {
    fprintf(stdout, "execute last command\n");
    fprintf(stdout, "arg count: %d\n", args->count);
    execute_command(opts, args, false);
  }
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
    }
  }
}

int main(int argc, char** argv, char** envp) {
  struct options opts;
  memset(&opts, 0, sizeof(struct options));
  opts.max_args = 5000;

  struct command_args args;
  memset(&args, 0, sizeof(struct command_args));
  args.count = 0;
  args.capacity = 10;
  args.args = malloc(args.capacity * sizeof(char*));
  if (args.args == NULL) {
    perror("malloc failed");
    exit(1);
  }

  parse_args(argc, argv, &opts, &args);
  run_xargs(&opts, &args, envp);
  
  return 0;
}
