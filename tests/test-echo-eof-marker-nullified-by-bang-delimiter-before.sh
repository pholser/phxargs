#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf "arg1!arg2!arg3!___!arg4!arg5!arg6" > "$test_input"

cat > "$expected_output" <<EOF
arg1 arg2 arg3 ___ arg4 arg5 arg6
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-d! -E ___' \
  ''
