#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
arg1 arg2 \
  arg3 arg4 \
    arg5
  arg6
EOF

cat > "$phx_expected_output" <<EOF
hello arg1 arg2   arg3 arg4     arg5
hello   arg6
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-I {} echo hello {}' \
  ''
