#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

echo "a a aaaaaaa a a a" > "$test_input"

cat > "$expected_output" <<EOF
a a
aaaaaaa a
a a
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-x -s 18' \
  ''
