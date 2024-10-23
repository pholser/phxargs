#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
arg1 arg2  arg3
arg4  arg5 arg6
EOF

cat > "$phx_expected_output" <<EOF
boo arg1 arg2  arg3
boo arg4  arg5 arg6
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 2 -I {} echo boo {}' \
  ''
