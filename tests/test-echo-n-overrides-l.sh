#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# -n given after -L cancels line mode; batching is by arg count not line.
# Input has two lines with 3 and 1 tokens. -L 1 -n 2 means -n 2 wins:
# invocations are (a, b) and (c, d), ignoring line boundaries.
cat > "$phx_test_input" <<EOF
a b c
d
EOF

cat > "$phx_expected_output" <<EOF
a b
c d
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-L 1 -n 2' \
  ''
