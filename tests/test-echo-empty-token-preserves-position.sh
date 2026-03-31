#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# An empty quoted token between other tokens is passed as an empty-string
# argument, preserving its position in the argument list.
# echo a "" b prints "a  b" (double space due to the empty arg).
printf "a '' b\n" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
a  b
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  ''
