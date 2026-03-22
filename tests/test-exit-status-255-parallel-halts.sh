#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# With -P 2, two children are in flight before halt is detected.
# Both exit 255, so the halt message should appear twice and exit status is 124.
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
phxargs: child exited with status 255 -- halting
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-P 2 -n 1' \
  './exit-255.sh' \
  124
