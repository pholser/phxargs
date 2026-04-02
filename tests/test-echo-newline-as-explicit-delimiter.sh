#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With -d $'\n', newline is the delimiter and spaces are literal token content.
# Each line becomes one token regardless of embedded spaces.
printf "a b\nc d\n" > "$phx_test_input"

printf "a b\nc d\n" > "$phx_expected_output"

cat > "$phx_expected_error" <<EOF
EOF

nl=$'\n'
./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  "-d '$nl' -n 1" \
  'echo' \
  0
