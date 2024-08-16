#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHUNK_SIZE 8192
#define INITIAL_ARGS_CAPACITY 10
#define DEFAULT_MAX_ARGS 5000

const char* default_command = "/bin/echo";
const char* option_flags = ":n:";

struct options {
    size_t max_args;
    char *max_args_endptr;

    bool trace;
};

struct command_args {
    size_t count;
    size_t capacity;
    char** args;
};

struct tokenizing_buffer {
    char* buffer;
    size_t size;
    size_t processed;
};

struct parser_state {
    bool in_quote;
    char quote_char;
    bool escape;
    size_t token_start;
};

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        perror("malloc failed");
        exit(1);
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
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

void free_args(struct command_args* args) {
    for (size_t i = 0; i < args->count; ++i) {
        free(args->args[i]);
    }
    free(args->args);
}

void reallocate_args_if_needed(struct command_args *args) {
    if (args->count >= args->capacity) {
        args->capacity *= 2;
        args->args =
            safe_realloc(args->args, args->capacity * sizeof(char*));
    }
}

void execute_command(
    struct options* opts,
    struct command_args* fixed_args,
    struct command_args* input_args) {

    pid_t pid = fork();
    handle_fork_error(pid);

    if (pid == 0) {
        // Child process
        size_t exec_args_count = fixed_args->count + input_args->count;
        char** exec_args = safe_malloc((exec_args_count + 1) * sizeof(char*));
        for (size_t i = 0; i < fixed_args->count; ++i) {
            exec_args[i] = fixed_args->args[i];
        }
        for (size_t i = 0; i < input_args->count; ++i) {
            exec_args[fixed_args->count + i] = input_args->args[i];
        }
        exec_args[exec_args_count] = NULL;

        if (opts->trace) {
            for (size_t i = 0; i < exec_args_count; ++i) {
                fprintf(
                    stderr,
                    "%s%c",
                    exec_args[i],
                    (i == exec_args_count - 1) ? '\n' : ' ');
            }
        }

        execvp(exec_args[0], exec_args);
        handle_execvp_error();
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        free_args(input_args);
    }
}

void add_argument(
    struct command_args* args,
    const char* new_arg) {

    reallocate_args_if_needed(args);

    args->args[args->count] = strdup(new_arg);
    args->count++;
}

void process_chunk(
    struct tokenizing_buffer* buf,
    struct options* opts,
    struct command_args* fixed_args,
    struct command_args* input_args,
    struct parser_state* pstate) {

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
            if (pstate->token_start < i) {
                buf->buffer[i] = '\0';
                add_argument(input_args, buf->buffer + pstate->token_start);

                if (input_args->count == opts->max_args) {
                    execute_command(opts, fixed_args, input_args);
                    allocate_args(input_args);
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

void run_xargs(
    struct options* opts,
    struct command_args* fixed_args) {

    struct tokenizing_buffer buf = {0};
    buf.buffer = safe_malloc(CHUNK_SIZE + 1);

    struct command_args input_args = {0};
    allocate_args(&input_args);

    struct parser_state pstate = {0};

    while ((buf.size = fread(
        buf.buffer + buf.processed,
        1,
        CHUNK_SIZE - buf.processed,
        stdin)) > 0) {

        buf.size += buf.processed;
        buf.buffer[buf.size] = '\0';
        process_chunk(&buf, opts, fixed_args, &input_args, &pstate);
    }
    if (ferror(stdin)) {
        fprintf(stderr, "phxargs: I/O error\n");
        exit(1);
    }

    if (buf.size > 0) {
        buf.buffer[buf.size] = '\0';
        add_argument(&input_args, buf.buffer);
    }

    if (input_args.count > 0) {
        execute_command(opts, fixed_args, &input_args);
    }

    free(buf.buffer);
}

long parse_number_arg(int opt, const char *arg, char **endptr) {
    errno = 0;
    long parsed = strtol(arg, endptr, 10);

    if (arg == *endptr
        || errno != 0
        || (errno == 0 && **endptr != '\0')) {

        fprintf(stderr, "phxargs: -%c %s: Invalid\n", opt, arg);
    }

    return parsed;
}

void parse_args(
    int argc,
    char** argv,
    struct options* opts,
    struct command_args* fixed_args) {

    int opt;
    while ((opt = getopt(argc, argv, option_flags)) != -1) {
        switch (opt) {
            case 'n':
                opts->max_args =
                    parse_number_arg(opt, optarg, &(opts->max_args_endptr));
                break;
            case 't':
                opts->trace = true;
                break;
            case ':':
                fprintf(stderr, "phxargs: -%c needs an argument\n", optopt);
                exit(1);
                break;
            case '?':
                fprintf(stderr, "phxargs: unknown option -%c\n", optopt);
                exit(1);
                break;
        }
    }

    if (optind == argc) {
        add_argument(fixed_args, default_command);
    } else {
        for (int i = optind; i < argc; ++i) {
            add_argument(fixed_args, argv[i]);
        }
    }
}

int main(int argc, char** argv) {
    struct options opts = {0};
    opts.max_args = DEFAULT_MAX_ARGS;

    struct command_args fixed_args = {0};
    allocate_args(&fixed_args);

    parse_args(argc, argv, &opts, &fixed_args);
    run_xargs(&opts, &fixed_args);

    free_args(&fixed_args);

    return 0;
}
