#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
arg1!arg2! arg3 and 4
EOF

cat > "$expected_output" <<EOF
hello arg1!arg2! arg3 and 4
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-I {} -d! echo hello {}' \
  ''