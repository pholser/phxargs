#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Input with mixed quotes and escape sequences
cat > "$phx_test_input" <<EOF
"a b c" 'd e f' g\\h\\i j\\ k\\"l\\'m \\'n
EOF

cat > "$phx_expected_output" <<EOF
a b c
d e f
ghi
j k"l'm
'n
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  ''
