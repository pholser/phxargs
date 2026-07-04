#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# GNU/POSIX xargs strips a trailing newline from the final token when using
# -d with a non-newline delimiter.  Input ends with \n; "baz" should arrive
# without it.
printf 'foo,bar,baz\n' > "$phx_test_input"

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
  '-d , -n 1' \
  'echo'
