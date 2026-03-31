#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

child_max=$(getconf CHILD_MAX)
too_large=$((child_max + 1))

cat > "$phx_test_input" <<EOF
a
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: -P $too_large: too large (system limit is $child_max)
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  "-P $too_large -n 1" \
  'echo' \
  1
