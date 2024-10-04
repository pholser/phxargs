#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

for i in $(seq 1 10000); do echo -n "arg$i "; done > "$test_input"

cat > "$expected_output" <<EOF
$(for i in $(seq 1 9999); do echo -n "arg$i "; done ; echo -n 'arg10000')
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '' \
  ''
