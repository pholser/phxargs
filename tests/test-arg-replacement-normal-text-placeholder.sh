#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
replace1
replace2
EOF

cat > "$expected_output" <<EOF
This is replace1, replacing a placeholder
This is replace2, replacing a placeholder
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-I PLACE echo This is PLACE, replacing a placeholder' \
  ''
