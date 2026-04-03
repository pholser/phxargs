# TODO

## Interactive tests: -t

Trace mode (`-t`) and prompt mode (`-p`) involve terminal interaction
that is difficult to test non-interactively.

## EINTR handling in getc loops

`getc` calls inside the tokenizers do not retry on `EINTR`. In practice
this is low risk — `getc` on a regular file or pipe is unlikely to be
interrupted — but strictly speaking a signal arriving mid-read could
cause a spurious tokenizer error. Assess whether adding `EINTR` retry
loops to the `getc` paths is warranted.

## CHANGELOG / version tags

No version history or git tags exist yet. Consider establishing a
versioning scheme (e.g. `v1.0.0`) and maintaining a CHANGELOG to
track user-visible changes across releases.
