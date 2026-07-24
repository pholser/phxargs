#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

: > "$phx_test_input"

actual_stderr=$(/usr/bin/env -i "$phx_build_dir/phxargs" -s 1073741824 echo < "$phx_test_input" 2>&1 >/dev/null)
actual_exit=$?

if [ "$actual_exit" -ne 1 ]; then
  echo "$phx_test_name: expected exit 1, got $actual_exit" >&2
  exit 1
fi

if ! printf '%s\n' "$actual_stderr" \
    | grep -qE '^phxargs: -s 1073741824: too large \(maximum [0-9]+ bytes\)$'; then
  echo "$phx_test_name: unexpected stderr: $actual_stderr" >&2
  exit 1
fi
