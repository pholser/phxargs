#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
arg1 arg2 arg3 ___ arg4 arg5
arg6
EOF

cat > "$expected_output" <<EOF
arg1 arg2 arg3
EOF

touch "$expected_error"

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-E ___' \
  ''