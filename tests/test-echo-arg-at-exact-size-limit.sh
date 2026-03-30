#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Verify the -s limit uses strict greater-than (new_length > max_length),
# so a command whose length equals the limit exactly is accepted.
#
# With a cleared environment:
#   /bin/echo   -> strlen("bin/echo") + 1 = 10 bytes
#   100-char arg -> 100 + 1               = 101 bytes
#   total                                 = 111 bytes
#
# -s 111 must accept (111 > 111 is false).
# -s 110 must reject (111 > 110 is true) -- covered by the companion test.

arg=$(printf 'a%.0s' {1..100})
printf '%s\n' "$arg" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
$arg
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-s 111' \
  ''
