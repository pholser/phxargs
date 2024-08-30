#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Input with special characters
cat > "$test_input" <<EOF
!@# \\$%^ &*()
EOF

cat > "$expected_output" <<EOF
!@# $%^ &*()
EOF

touch "$expected_error"

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '' \
  ''
