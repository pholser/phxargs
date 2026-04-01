% PHXARGS(1) phxargs | User Commands
%
% April 2026

# NAME

phxargs - build and execute command lines from standard input

# SYNOPSIS

**phxargs** [*options*] [*command* [*initial-arguments*]]

# DESCRIPTION

**phxargs** reads items from standard input (or a file specified with **-a**)
and executes *command* with those items appended as arguments. If no *command*
is given, **/bin/echo** is used.

Input is split into arguments by whitespace by default, with support for
quoting and backslash escaping. The input delimiter can be changed with **-0**
or **-d**.

Execution continues until input is exhausted or a child exits with status 255.

# OPTIONS

**-0**
:   Input items are terminated by a NUL character instead of whitespace.
    Disables **-d**.

**-a** *file*
:   Read input items from *file* instead of standard input.

**-d** *delim*
:   Use *delim* as the input item delimiter. *delim* must be a single
    character. Disables **-0** and **-E**.

**-E** *marker*
:   Stop processing input when an input item equals *marker* (logical
    end-of-file). Has no effect when **-I**, **-0**, or **-d** is in use.

**-I** *replstr*
:   Replace occurrences of *replstr* in *initial-arguments* with each
    input item. Implies **-L 1** and **-x**. Nullifies **-E**.

**-L** *max-lines*
:   Use at most *max-lines* non-empty input lines per command invocation.
    Implies **-x**. Mutually exclusive with **-I** and **-n**.

**-n** *max-args*
:   Use at most *max-args* arguments per command invocation.
    Mutually exclusive with **-I** and **-L**.

**-o**
:   Reopen standard input as **/dev/tty** in the child process before
    executing the command. Useful for commands that need to interact with
    the terminal.

**-p**
:   Prompt the user before executing each command line. A line beginning
    with **y** or **Y** confirms execution. Implies **-t**. Mutually
    exclusive with **-P** *max-procs* greater than 1.

**-P** *max-procs*
:   Run up to *max-procs* command invocations in parallel. The default is
    1 (serial execution). A value of **0** means unlimited parallelism,
    bounded only by the system's child process limit. If any child exits
    with status 255, no further invocations are started.

    While running, send **SIGUSR1** to **phxargs** to increase the
    parallelism limit by 1; send **SIGUSR2** to decrease it (minimum 1).

    Mutually exclusive with **-p**.

**-r**
:   Do not run the command if there are no input items. By default,
    *command* is run once even when input is empty.

**-s** *max-chars*
:   Limit the size of each command line to *max-chars* characters,
    counting the command, its arguments, and terminating NUL bytes.
    Defaults to the lesser of 128 KiB and the system ARG_MAX (minus
    environment overhead).

**-t**
:   Print each command line to standard error before executing it.

**-x**
:   Exit if a constructed command line would exceed the size limit set by
    **-s**. Always implied by **-I** and **-L**.

# EXIT STATUS

**0**
:   All invocations succeeded.

**123**
:   One or more command invocations exited with a non-zero status between
    1-125 or 128-254.

**124**
:   A command invocation exited with status 255; no further invocations
    were started.

**125**
:   A command invocation was killed by a signal, or an internal error
    occurred in **phxargs** itself.

**126**
:   The command was found but could not be executed (permission denied or
    not a valid executable).

**127**
:   The command was not found.

When multiple invocations produce different non-zero exit statuses, the
most severe wins (125 > 124 > 123 > 126/127).

# EXAMPLES

Delete all **.tmp** files under the current directory:

    find . -name '*.tmp' | phxargs rm

Pass each filename to a command one at a time:

    find . -name '*.c' | phxargs -n 1 wc -l

Replace a placeholder in a template command:

    echo '/tmp/workdir' | phxargs -I {} cp -r {} /backup

Process NUL-delimited output from **find -print0**:

    find . -name '*.log' -print0 | phxargs -0 gzip

Run up to 4 compilations in parallel:

    echo src/*.c | tr ' ' '\n' | phxargs -n 1 -P 4 cc -c

# SEE ALSO

find(1), xargs(1)
