# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-07-25

### Added

- Tests for `-s` bounds checking (too-small and too-large command length).
- Test for exit-255 halt in `-I` (replace-mode) confirming only the first
  argument is processed before phxargs stops.
- Test for `process_pool_submit` capacity doubling: `-P 0` with 17 tokens
  exercises the realloc path that grows the PID array beyond its initial
  capacity.
- `lizard` and `module-graph` CMake targets for cyclomatic-complexity
  analysis and module-dependency visualisation.
- `lizard` and GitHub Release CI jobs.

### Changed

- Signal tests now use a slow child command (`sleep 0.5`) so SIGUSR1/SIGUSR2
  arrive while phxargs is still running; previously, fast `echo` children
  finished before the signal was delivered, leaving the signal-handling paths
  untested.
- `command_execute_async` (CC 12) split into `write_trace` (CC 7) and
  `exec_child` (CC 5), bringing all functions under the CC > 10 warning
  threshold. `exec_child` is correctly annotated `__attribute__((noreturn))`.
- Sanitizers CI job restricted to Linux; macOS Apple Clang ASan intercepts
  `pthread_init` and `openpty` in ways that crash the test harness before
  any test code runs.

### Fixed

- Removed dead `on_input_boundary` guard in `handle_no_token_char`: the
  condition could never be true because `line_has_token` is reset at the
  start of every `next_space_token` call.
- `exec_child` parameter tightened to `const command*` (cppcheck finding).
- Removed redundant `#include <stddef.h>` from `str.c` (IWYU finding).

## [1.1.0] - 2026-07-04

### Changed

- **`-d` and `-0` now strip a trailing newline from the final token** at EOF,
  matching GNU xargs and POSIX-based behaviour. Input that previously yielded
  a last token ending in `\n` (e.g. `printf 'a,b,c\n' | phxargs -d ,`) now
  yields `c` instead of `c\n`. This does not apply when the delimiter is
  itself a newline (`-d $'\n'`), since in that case newlines are already
  consumed as delimiters.

## [1.0.0] - 2026-05-09

Initial release of phxargs, a clean-room POSIX xargs implementation.

### Features

- Full POSIX xargs option set: `-E`, `-I`, `-L`, `-n`, `-p`, `-s`, `-t`, `-x`
- GNU xargs extensions: `-0`, `-a`, `-d`, `-P`, `-r`
- BSD xargs extension: `-o` (reopen stdin as `/dev/tty` in child)
- Parallel execution (`-P`) with runtime pool resizing via SIGUSR1/SIGUSR2
- Space tokenizer with full quoting and backslash-escape support
- Single-character delimiter tokenizer (`-d`, `-0`)
- Structured exit codes (0, 123–127) matching GNU xargs semantics

### Platform support

- Linux (Ubuntu), macOS, FreeBSD

### Quality

- 124 tests covering options, edge cases, parallel execution, exit status,
  prompt/trace/tty modes, and tokenizer behaviour
- AddressSanitizer and UndefinedBehaviorSanitizer CI builds
- cppcheck static analysis
- libFuzzer fuzz targets for both tokenizers with seed corpus
- gcov/lcov code coverage (~92% line coverage)
- `-Werror` across Debug, Release, and sanitizer builds
- CMake install target for binary and man page

### Documentation

- Man page with full option reference, examples, exit status table, and
  POSIX compliance notes
