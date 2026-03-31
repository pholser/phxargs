#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# One of the inputs will cause a non-zero exit; the rest should still complete.
cat > "$phx_test_input" <<EOF
one
two
three
EOF

cat > "$phx_expected_output" <<EOF
one
three
EOF

cat > "$phx_expected_error" <<EOF
EOF

# sh -c 'echo "$1"; [ "$1" != two ]' -- ARG: echoes the arg and exits non-zero for "two"
./run-unordered-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-P 2 -n 1' \
  './echo-unless-two.sh' \
  123
