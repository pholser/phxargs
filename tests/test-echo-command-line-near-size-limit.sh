#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

echo "a a aaaaaaa a a a" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
a a
aaaaaaa
a a a
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-x -s 18' \
  ''
