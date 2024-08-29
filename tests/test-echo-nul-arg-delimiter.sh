#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf "arg1\0arg2\0arg3\0" > "$test_input"

cat > "$expected_output" <<EOF
arg1 arg2 arg3
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_err" \
  '-0' \
  ''
