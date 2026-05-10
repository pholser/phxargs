# TODO

## EINTR handling in getc loops

Not warranted. `getc` on a regular file or pipe is unlikely to be
interrupted by a signal in practice, and both glibc and macOS libc
restart `getc` automatically on `EINTR` when `SA_RESTART` is set (which
it is for our signal handlers). The only realistic exposure is on BSD
without `SA_RESTART`, but the window is tiny and a spurious short-read
would surface as a tokenizer error that phxargs already reports cleanly.
The complexity of retry loops outweighs the benefit. Closed — no action.

