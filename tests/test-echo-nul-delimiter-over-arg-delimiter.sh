#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf "arg1,arg2,arg3\0" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
arg1,arg2,arg3
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-d, -0' \
  ''
