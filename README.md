# `phxargs` - an `xargs` clone

Thought I'd try something with just C and its standard library after all
these years.

See [ARCHITECTURE.md](ARCHITECTURE.md) for a design overview.

## Dependencies

Required:

- C99-compatible compiler (`clang` or `gcc`)
- [CMake](https://cmake.org/) 3.10 or later
- [Bash](https://www.gnu.org/software/bash/) — for the shell-based tests
- [Check](https://libcheck.github.io/check/) — for the C unit tests
- [expect](https://core.tcl-lang.org/expect/index) — for the interactive (`*.exp`) tests
- [pandoc](https://pandoc.org/) — for man page generation

Optional:

- [cppcheck](https://cppcheck.sourceforge.io/) — static analysis (`cppcheck` target)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) — static analysis (`clang-tidy` target)
- [include-what-you-use](https://include-what-you-use.org/) — include analysis (`iwyu` target)
- [lcov](https://github.com/linux-test-project/lcov) + genhtml — coverage reports (`coverage` target)
- clang with libFuzzer — fuzz targets

On macOS with Homebrew:

```sh
brew install bash cmake check expect pandoc
```

## Building

Configure (only needed once, or after changes to `CMakeLists.txt`):

```sh
cmake -S . -B build
```

Build:

```sh
cmake --build build
```

The `phxargs` binary and `phxargs.1` man page are placed in `build/`.

## Installing

```sh
cmake --install build --prefix /usr/local
```

## Running tests

```sh
ctest --test-dir build --output-on-failure
```

Tests include shell-based tests (`tests/test-*.sh`, `tests/test-*.exp`),
a compatibility test against the system `xargs`, and C unit tests using
the Check framework.

## Static analysis

```sh
cmake --build build --target cppcheck
cmake --build build --target clang-tidy
```

## Include analysis (IWYU)

```sh
cmake --build build --target iwyu
```

## Coverage

Build and generate an HTML report in `build-cov/coverage/`:

```sh
cmake -S . -B build-cov -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cov --target coverage
```

## Sanitizer build

Build with AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-san -DENABLE_SANITIZERS=ON
cmake --build build-san
ctest --test-dir build-san --output-on-failure
```

## Fuzzing

Build the libFuzzer fuzz targets (requires a Clang with libFuzzer support):

```sh
cmake -S . -B build-fuzz -DENABLE_FUZZING=ON -DCMAKE_C_COMPILER=clang
cmake --build build-fuzz --target fuzz_space_tokenizer fuzz_delim_tokenizer
./build-fuzz/fuzz_space_tokenizer -max_total_time=60
./build-fuzz/fuzz_delim_tokenizer -max_total_time=60
```

## Usage

```
phxargs [options] [command [initial-arguments]]
```

Reads arguments from standard input (or a file with `-a`) and executes
`command` with those arguments appended. If no `command` is given,
`/bin/echo` is used.

## Supported options

| Option | Description |
|--------|-------------|
| `-0` | Input items are terminated by a NUL character instead of whitespace. Disables `-d`. |
| `-a file` | Read arguments from `file` instead of standard input. |
| `-d delim` | Use `delim` as the input delimiter (single character). Disables `-0`. |
| `-E marker` | Stop processing input when a line equals `marker` (logical end-of-file). Nullified by `-I` or a NUL/custom delimiter. |
| `-I replstr` | Replace occurrences of `replstr` in the initial arguments with input lines. Implies `-L 1` and `-x`; nullifies `-E`. |
| `-L max-lines` | Use at most `max-lines` non-empty input lines per command invocation. Implies `-x`. Mutually exclusive with `-I` and `-n`. |
| `-n max-args` | Use at most `max-args` arguments per command invocation. Mutually exclusive with `-I` and `-L`. |
| `-o` | Reopen stdin as `/dev/tty` in the child process before executing the command. |
| `-p` | Prompt the user before executing each command line. Implies `-t`. Mutually exclusive with `-P max-procs > 1`. |
| `-P max-procs` | Run up to `max-procs` command invocations in parallel. Defaults to 1 (serial). `0` means unlimited. If a child exits with status 255, no further invocations are started. Send `SIGUSR1` to increase the limit by 1; `SIGUSR2` to decrease it (minimum 1). Mutually exclusive with `-p`. |
| `-r` | Do not run the command if standard input is empty. |
| `-s max-chars` | Limit the length of each command line to `max-chars` characters. |
| `-t` | Print each command to stderr before executing it. |
| `-x` | Terminate if a constructed command line would exceed the size limit set by `-s`. |

## Exit status

| Status | Meaning |
|--------|---------|
| `0` | All invocations succeeded. |
| `123` | One or more command invocations exited with a status between 1–125 or 128–254. |
| `124` | A command invocation exited with status 255; no further invocations were started. |
| `125` | A command invocation was killed by a signal, or an internal error occurred in phxargs itself. |
| `126` | The command was found but could not be executed (passed through from the child). |
| `127` | The command was not found (passed through from the child). |

When multiple invocations produce different non-zero statuses, the most severe wins (125 > 124 > 123).
