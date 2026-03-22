#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Serial execution: the first child exits 1 (-> 123, severity 2),
# the second kills itself with SIGTERM (-> 125, severity 4).
# The accumulator should upgrade to 125 because it is more severe.
cat > "$phx_test_input" <<EOF
fail
signal
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
