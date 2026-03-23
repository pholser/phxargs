#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Simplest case: single invocation, child exits non-zero (1).
# phxargs maps any exit code in [1,125] to 123.
cat > "$phx_test_input" <<EOF
fail
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  './dispatch-failure.sh' \
  123
