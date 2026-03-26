#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# -d ' ' splits on literal space only; tabs and newlines are token content.
# Input "a b<TAB>c" produces two tokens: "a" and "b<TAB>c".
# echo receives both in one invocation and prints "a b<TAB>c".
printf "a b\tc" > "$phx_test_input"

printf "a b\tc\n" > "$phx_expected_output"

cat > "$phx_expected_error" <<EOF
EOF

/usr/bin/env -i ./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  "-d ' '" \
  '' \
  0
