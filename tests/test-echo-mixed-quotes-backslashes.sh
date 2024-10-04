#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Input with mixed quotes and escape sequences
cat > "$test_input" <<EOF
"a b c" 'd e f' g\\h\\i j\\ k\\"l\\'m \\'n
EOF

cat > "$expected_output" <<EOF
a b c
d e f
ghi
j k"l'm
'n
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-n 1' \
  ''
