#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Serial execution: first child is killed by SIGTERM (-> 125, severity 4),
# second child exits 1 (-> 123, severity 2).
# The accumulator should keep 125 because later lower-severity does not win.
cat > "$phx_test_input" <<EOF
signal
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
  125
