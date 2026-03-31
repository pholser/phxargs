# `phxargs` - an `xargs` clone

Thought I'd try something with just C and its standard library after all
these years.

## Dependencies

- C99-compatible compiler (e.g. `clang` or `gcc`)
- [CMake](https://cmake.org/) 3.10 or later
- [Bash](https://www.gnu.org/software/bash/) -- required for running the shell-based tests
- [Check](https://libcheck.github.io/check/) -- required for building and running the C unit tests
- [expect](https://core.tcl-lang.org/expect/index) -- required for running the interactive (`*.exp`) tests
- [include-what-you-use](https://include-what-you-use.org/) (`iwyu`) -- optional, for include analysis

On macOS with Homebrew:

```sh
brew install bash cmake check expect include-what-you-use
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

The `phxargs` binary is placed in `build/`.

## Running tests

```sh
cd build && ctest --output-on-failure
```

Or via the custom target:

```sh
cmake --build build --target run_tests
```

Tests include both shell-based tests (`tests/test-*.sh`, `tests/test-*.exp`) and C unit tests using the Check framework.

## Include analysis (IWYU)

If `include-what-you-use` is installed:

```sh
cmake --build build --target iwyu
```

## Sanitizer build

To build with AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-san -DENABLE_SANITIZERS=ON
cmake --build build-san
ctest --test-dir build-san --output-on-failure
```

## Fuzzing

To build the libFuzzer fuzz targets (requires a Clang with libFuzzer support):

```sh
cmake -S . -B build-fuzz -DENABLE_FUZZING=ON
cmake --build build-fuzz --target fuzz_space_tokenizer --target fuzz_delim_tokenizer
./build-fuzz/fuzz_space_tokenizer -max_total_time=60
./build-fuzz/fuzz_delim_tokenizer -max_total_time=60
```

## Usage

```
phxargs [options] [command [initial-arguments]]
```

Reads arguments from standard input (or a file with `-a`) and executes `command` with those arguments appended. If no `command` is given, `/bin/echo` is used.

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
| `125` | A command invocation was killed by a signal. |
| `126` | The command was found but could not be executed (passed through from the child). |
| `127` | The command was not found (passed through from the child). |

When multiple invocations produce different non-zero statuses, the most severe wins (125 > 124 > 123).
