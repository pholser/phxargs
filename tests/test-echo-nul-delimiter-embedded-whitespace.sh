#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# NUL delimiter preserves embedded spaces and newlines within tokens
printf 'hello world\0foo\tbar\0line1\nline2\0' > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
hello world
foo	bar
line1
line2
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-0 -n 1' \
  'echo'
