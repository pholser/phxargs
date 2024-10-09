#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
a  w r   t   ff
sdfsd s
EOF

cat > "$expected_output" <<EOF
a w r t ff sdfsd s
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  "-a $test_input" \
  ''
