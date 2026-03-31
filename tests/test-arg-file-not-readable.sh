#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

phx_unreadable_file=$(make_temp "$phx_test_output_dir" unreadable)
chmod 000 "$phx_unreadable_file"

phx_actual_output="$phx_test_output_dir/phxargs-$phx_test_name.out"
phx_actual_error="$phx_test_output_dir/phxargs-$phx_test_name.err"

/usr/bin/env -i "$phx_build_dir/phxargs" \
  -a "$phx_unreadable_file" \
  > "$phx_actual_output" \
  2> "$phx_actual_error" \
  < "$phx_test_input"
actual_exit=$?

chmod 600 "$phx_unreadable_file"

if [ "$actual_exit" -ne 1 ]; then
  echo "$phx_test_name: expected exit 1, got $actual_exit" >&2
  exit 1
fi

if [ -s "$phx_actual_output" ]; then
  echo "$phx_test_name: expected empty stdout" >&2
  exit 1
fi

expected_error="phxargs: $phx_unreadable_file: Permission denied"
actual_error=$(cat "$phx_actual_error")

if [ "$actual_error" != "$expected_error" ]; then
  echo "$phx_test_name: stderr differs" >&2
  diff <(echo "$expected_error") <(echo "$actual_error") >&2
  exit 1
fi
