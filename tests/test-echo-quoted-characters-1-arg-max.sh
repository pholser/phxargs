#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
   "a\\b  c d" '   e f   ' g\\h\\i
EOF

cat > "$expected_output" <<EOF
a\b  c d
   e f   
ghi
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_err" \
  '-n 1' \
  ''
