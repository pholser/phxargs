#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

long_arg=$(printf 'a%.0s' {1..2047})
echo "$long_arg" > "$test_input"

touch "$expected_output"

cat > "$expected_error" <<EOF
phxargs: command too long
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-s 2057' \
  ''