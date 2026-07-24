#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

: > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: -s 1: too small (minimum 5 bytes)
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-s 1' \
  'echo' \
  1
