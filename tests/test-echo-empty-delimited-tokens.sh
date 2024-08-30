#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Input with empty tokens (e.g., multiple consecutive delimiters)
printf "a,,b,,c" > "$test_input"

cat > "$expected_output" <<EOF
a  b  c
EOF

touch "$expected_error"

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-d ,' \
  ''
