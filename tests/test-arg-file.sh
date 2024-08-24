#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
a
b c
d e f
EOF

cat > "$expected_output" <<EOF
a b c d e f
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  /dev/null \
  "$expected_output" \
  "$expected_err" \
  "-a $test_input" \
  ''
