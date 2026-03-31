#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

cat > "$phx_test_input" <<EOF
one
two
three
four
five
EOF

cat > "$phx_expected_output" <<EOF
one
two
three
four
five
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-unordered-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-P 2 -n 1' \
  'echo'
