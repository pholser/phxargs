# TODO

## EINTR handling in getc loops

`getc` calls inside the tokenizers do not retry on `EINTR`. In practice
this is low risk — `getc` on a regular file or pipe is unlikely to be
interrupted — but strictly speaking a signal arriving mid-read could
cause a spurious tokenizer error. Assess whether adding `EINTR` retry
loops to the `getc` paths is warranted.

