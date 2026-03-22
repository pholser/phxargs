#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
a
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
EOF

# kill sends SIGTERM (15) to the process; phxargs should exit 125
./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  './self-sigterm.sh' \
  125
