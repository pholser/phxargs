#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
arg1 arg2 arg3\ \ \ \
arg4 arg5 arg6\ \
arg7 arg8 arg9\
arg10 arg11
EOF

cat > "$phx_expected_output" <<EOF
arg1 arg2 arg3   arg4 arg5 arg6 arg7 arg8 arg9arg10 arg11
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-L 2' \
  ''
