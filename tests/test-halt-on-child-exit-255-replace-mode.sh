#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

printf 'a\nb\n' > "$phx_test_input"

actual_stdout=$(/usr/bin/env -i "$phx_build_dir/phxargs" -I '{}' \
  sh -c 'echo {}; exit 255' < "$phx_test_input" 2>/dev/null)
actual_exit=$?

expected_stdout="a"

if [ "$actual_exit" -ne 124 ]; then
  echo "$phx_test_name: expected exit 124, got $actual_exit" >&2
  exit 1
fi

if [ "$actual_stdout" != "$expected_stdout" ]; then
  printf '%s: expected stdout |%s|, got |%s|\n' \
    "$phx_test_name" "$expected_stdout" "$actual_stdout" >&2
  exit 1
fi
