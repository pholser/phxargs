#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# A very long argument, near boundary limits
long_arg=$(printf 'a%.0s' {1..2047})
echo "$long_arg" > "$test_input"

cat > "$expected_output" <<EOF
$long_arg
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_err" \
  '-s 2058' \
  ''
