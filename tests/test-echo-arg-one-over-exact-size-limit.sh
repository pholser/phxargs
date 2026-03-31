#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# Companion to test-echo-arg-at-exact-size-limit: one byte below the required
# limit must be rejected.
#
# With a cleared environment:
#   /bin/echo    -> 10 bytes
#   100-char arg -> 101 bytes
#   total        = 111 bytes
#
# -s 110 must reject (111 > 110 is true).

arg=$(printf 'a%.0s' {1..100})
printf '%s\n' "$arg" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: command too long
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-s 110' \
  '' \
  1
