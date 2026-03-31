#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With multiple parallel invocations mixing signal, failure, and success,
# signal (125, severity 4) should beat failure (123, severity 2) and
# success (0, severity 0).
cat > "$phx_test_input" <<EOF
succeed
fail
signal
fail
succeed
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
  '-P 5 -n 1' \
  './dispatch-failure.sh' \
  125
