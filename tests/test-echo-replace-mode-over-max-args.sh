#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
arg1 arg2  arg3
arg4  arg5 arg6
EOF

cat > "$expected_output" <<EOF
boo arg1 arg2  arg3
boo arg4  arg5 arg6
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-n 2 -I {} echo boo {}' \
  ''
