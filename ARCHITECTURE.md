# Architecture

## Overview

phxargs is structured as a pipeline of four layered concerns:

```
input source → tokenizer → xargs mode (command + process pool) → child
processes
```

Each layer has a single responsibility and communicates through a narrow
interface. The two places where runtime polymorphism is needed -- tokenization
strategy and execution mode -- use C vtable structs rather than conditional logic scattered through the call sites.

## Source map

```
src/
  main.c              entry point
  options.c/.h        argument parsing and normalization
  xargs.c/.h          top-level orchestration
  xargs_mode.c/.h     vtable base for execution modes; owns tokenizer,
                        command, pool
  appender_mode.c/.h  default mode: accumulate tokens, flush on limit
  replacer_mode.c/.h  -I mode: substitute placeholder, execute per token
  command.c/.h        argv construction, size accounting, fork/exec
  command_args.c/.h   growable argv list with length tracking
  process_pool.c/.h   parallel child management, signal handling
  tokenizer.c/.h      vtable base for tokenizer implementations
  space_tokenizer.c/.h  default whitespace/quote/backslash tokenizer
  delim_tokenizer.c/.h  -d/-0 single-character delimiter tokenizer
  arg_source.c/.h     stdin vs -a file abstraction
  buffer.c/.h         growable byte buffer (used by tokenizers)
  str.c/.h            str_replace: placeholder substitution for -I mode
  exit_codes.h        exit status constants
  util.c/.h           malloc/fork/exec wrappers, sysconf, tty helpers
```

## Layer details

### options

`options_create` calls `getopt` and then `configure_options`, which applies the
inter-option implications that POSIX and GNU xargs define:

- `-L` implies `-x`
- `-I` implies `-L 1`, `-x`, nullifies `-E`, and forces `\n` as delimiter if no
  other delimiter is in force
- `-p` and `-P > 1` are mutually exclusive

The `options` struct is consumed during construction of `xargs` and then
destroyed; nothing downstream holds a pointer to it.

### tokenizer

The tokenizer vtable (`tokenizer_ops`) has two function pointers: `next_token`
and `destroy_impl`. The concrete struct (`tokenizer`) holds the shared growable
`buffer` and error state; each implementation embeds a pointer back to it.

`next_token` returns a `const char*` into the shared buffer on success, `NULL`
on EOF or error. After returning `NULL`, callers check `tokenizer_get_error` to
distinguish clean EOF from I/O error, backslash-at-EOF, and unterminated quote.

Both tokenizer implementations accept an `on_input_boundary` callback (`input_boundary_fn`)
that fires whenever a logical line boundary is consumed. This is the mechanism
by which `-L` tracks line counts without the tokenizer needing to know about
command limits.

**SpaceTokenizer** implements a six-state machine:

```
NO_TOKEN → (non-ws) → IN_TOKEN → (ws) → NO_TOKEN          [token complete]
                               → (\n) → NO_TOKEN           [token complete, on_input_boundary]
         → (quote)  → IN_QUOTED_TOKEN                      [quoted token]
         → (\\)     → NO_TOKEN_ESCAPE → IN_TOKEN           [escape outside token]
IN_TOKEN → (quote)  → IN_TOKEN_QUOTED → IN_TOKEN           [mid-token quote]
         → (\\)     → IN_TOKEN_ESCAPE → IN_TOKEN           [mid-token escape]
```

The logical EOF marker (`-E`) is checked when a token completes: if it matches,
`next_token` returns `NULL` as if EOF had been reached.

**DelimTokenizer** is simpler: accumulate bytes until the delimiter is seen or
EOF, fire `on_input_boundary` on each delimiter, return the accumulated token.

### xargs_mode

`xargs_mode` is the base struct shared by both execution modes. It owns:

- the `FILE*` input source (stdin or `-a` file)
- the `tokenizer` (space or delim, chosen during construction)
- the `command` (argv builder)
- the `process_pool`
- an opaque `impl` pointer back to the concrete mode struct

The vtable (`xargs_mode_ops`) has two entries: `run` and `destroy_impl`.
`xargs_mode` exposes the operations that both modes need — `next_token`,
`add_input_argument`, `execute_command`, `drain`, etc. — so that mode
implementations never reach through to the inner structs directly.

**AppenderMode** implements the default accumulation loop:

```
for each token:
  if adding token would exceed limits: execute current batch
  add token
  if should flush after add (line count or arg count at max): execute
after loop:
  if input was present and args remain: execute final batch
  if no input and -r not set: execute once (empty invocation)
drain pool
```

**ReplacerMode** (`-I`) is simpler: for each token, substitute the placeholder
into the fixed args and execute immediately. No batching.

### command

`command` tracks the argv being built and enforces the limits set by `-n`, `-s`,
and `-L`. It owns two `command_args` lists: `fixed_args` (from the command line)
and `input_args` (accumulated from stdin).

`command_length` returns the sum of env_length + fixed_args_length +
input_args_length. This is compared against `max_length` (derived from
`sysconf(_SC_ARG_MAX)` minus a conservative safety margin) to detect overrun.

`command_execute_async` calls `fork()`. In the parent, `recycle_command` clears
`input_args` and resets `line_count` before returning. In the child,
`build_exec_args` assembles the final `char**` and `execvp` replaces the process
image. The child path never returns.

For `-I` mode, `command_replace_args` calls `str_replace` on each fixed arg and
builds `replaced_fixed_args`; `build_exec_args` uses that list instead of
`fixed_args`.

### process_pool

`process_pool` holds an array of in-flight child PIDs and enforces the `-P`
concurrency limit.

`process_pool_wait_if_full` blocks the parent by calling `waitpid(-1, 0)` until
the count drops below `max_procs`. `reap_one` retries on `EINTR`.
`process_pool_drain` calls `reap_one` until the pool is empty.

Exit status from each reaped child is mapped to a severity level (0–6) and
accumulated: the highest severity seen across all children becomes the final
exit code. Exit status 255 from any child sets the `halt` flag; subsequent
`execute_command` calls are silently skipped.

SIGUSR1 and SIGUSR2 handlers increment/decrement `sig_atomic_t` counters.
`apply_signal_adjustments` (called at the top of `wait_if_full`) drains those
counters and adjusts `max_procs` accordingly, clamped to `[1, CHILD_MAX]`.
Both handlers are installed with `SA_RESTART` so that interrupted `waitpid`
calls restart automatically.

### arg_source

A thin wrapper: `arg_source_open` returns `stdin` when no `-a` path is given,
or opens the file and exits on failure. `arg_source_close` skips `fclose` for
`stdin`.

### buffer

A heap-allocated growable byte array used as the token accumulation buffer
inside each tokenizer. `buffer_put` doubles capacity on overflow.
`buffer_reset` resets the position without freeing, allowing the buffer to be
reused across tokens within a single `next_token` call.

## Key design decisions

**Vtables over conditionals.** Both the tokenizer and the execution mode use
struct-of-function-pointers vtables rather than `switch` statements in shared
code. This keeps `appender_mode_run` and `replacer_mode_run` independent and
readable, and makes the tokenizer implementations fully self-contained.

**`xargs_mode` as a mediating layer.** Neither `appender_mode` nor
`replacer_mode` holds direct pointers to `command`, `tokenizer`, or
`process_pool`. All access goes through `xargs_mode`'s interface. This means
the two modes only depend on the operations they actually use, not on the
internal structure of the objects below them.

**`on_input_boundary` callback for line counting.** Rather than having the tokenizer
expose a "lines consumed" counter and the command poll it, the tokenizer calls
back into the command on each line boundary. This keeps the coupling unidirectional
and avoids a polling loop.

**fork/exec ownership in `command`.** The decision to put `fork`, `execvp`, and
`recycle_command` together in `command_execute_async` means the pool never sees
the child process details. The pool only ever sees PIDs. This made it
straightforward to add trace output, prompt confirmation, and `/dev/tty`
reopening in the child path without touching the pool.

**`line_has_token` as a local in `next_space_token`.** The flag that controls
whether `on_input_boundary` fires is local to each invocation of `next_space_token`,
not a field on the tokenizer. This means the tokenizer correctly fires
`on_input_boundary` only for lines that actually contained a token, regardless of whether
the previous call ended mid-line or at a line boundary.
