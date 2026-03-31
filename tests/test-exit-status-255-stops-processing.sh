#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With several inputs, the first invocation exits 255.
# phxargs should stop processing remaining inputs and exit 124.
cat > "$phx_test_input" <<EOF
a
b
c
d
e
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: child exited with status 255 -- halting
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  './exit-255.sh' \
  124
