#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With two parallel failures of different severities, the higher one should win.
# "signal" -> killed by SIGTERM (-> 125, severity 4)
# "fail"   -> exits 1          (-> 123, severity 2)
# Expected: 125 (signal takes priority over plain failure)
cat > "$phx_test_input" <<EOF
signal
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
  '-P 2 -n 1' \
  './dispatch-failure.sh' \
  125
