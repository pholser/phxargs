#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

cat > "$phx_test_input" <<EOF
a b c d e
EOF

cat > "$phx_expected_output" <<EOF
a b
c d
e
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 2 --' \
  'echo'
