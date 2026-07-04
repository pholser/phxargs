#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# GNU/POSIX xargs strips a trailing newline from the final token when using
# -0.  Input ends with \n (no trailing NUL); "baz" should arrive without it.
printf 'foo\0bar\0baz\n' > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
foo
bar
baz
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
