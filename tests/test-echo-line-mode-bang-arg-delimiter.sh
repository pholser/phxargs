#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf "arg1!arg2!arg3!arg4!\narg5\n!arg6!arg7!arg8" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
arg1 arg2 arg3
arg4 
arg5
 arg6
arg7 arg8
EOF

touch "$phx_expected_error"

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-d! -L 3' \
  ''
