#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# The process pool PID array starts with capacity = max_procs (or 16 for -P 0).
# With -P 0 the pool never blocks, so all children are submitted before any
# are reaped.  The 17th submit hits count(16) == capacity(16) and triggers
# the capacity-doubling realloc in process_pool_submit.
seq 17 > "$phx_test_input"

actual_stdout=$(/usr/bin/env -i "$phx_build_dir/phxargs" -P 0 -n 1 echo \
  < "$phx_test_input" 2>/dev/null)
actual_exit=$?

if [ "$actual_exit" -ne 0 ]; then
  echo "$phx_test_name: expected exit 0, got $actual_exit" >&2
  exit 1
fi

expected=$(seq 17)
if [ "$(printf '%s\n' $actual_stdout | sort -n)" != "$(printf '%s\n' $expected | sort -n)" ]; then
  echo "$phx_test_name: unexpected output" >&2
  printf 'expected: %s\n' "$expected" >&2
  printf 'actual:   %s\n' "$actual_stdout" >&2
  exit 1
fi
