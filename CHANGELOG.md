# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
