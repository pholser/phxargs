#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With multiple parallel invocations mixing success and failure, failure
# (123, severity 2) should beat success (0, severity 0).
cat > "$phx_test_input" <<EOF
succeed
fail
succeed
fail
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-unordered-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-P 4 -n 1' \
  './dispatch-failure.sh' \
  123
