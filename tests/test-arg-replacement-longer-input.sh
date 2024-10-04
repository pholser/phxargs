#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Long input with replacements
cat > "$test_input" <<EOF
$(printf "item%.0s\n" {1..100})
EOF

cat > "$expected_output" <<EOF
$(printf "Processed item%.0s\n" {1..100})
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-I {} echo Processed {}' \
  ''
