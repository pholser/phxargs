#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

long_arg=$(printf 'a%.0s' {1..2047})
echo "$long_arg" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
$long_arg
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-s 2058' \
  ''
