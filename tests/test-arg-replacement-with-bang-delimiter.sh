#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

printf "arg1!arg2! arg3 and 4" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
hello arg1
hello arg2
hello  arg3 and 4
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-I {} -d! echo hello {}' \
  ''
