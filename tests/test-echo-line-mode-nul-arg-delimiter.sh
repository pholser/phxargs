#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf "arg1\0arg2\0arg3\0arg4\0arg5\0arg6" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
arg1 arg2
arg3 arg4
arg5 arg6
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-0 -L 2' \
  ''
