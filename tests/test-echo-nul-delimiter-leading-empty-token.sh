#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# A leading NUL byte produces an empty first token with -0.
# echo receives ("", "a", "b") and prints them space-separated.
printf "\0a\0b" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
 a b
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-0' \
  ''
