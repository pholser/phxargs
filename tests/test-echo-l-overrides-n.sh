#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# -L given after -n cancels max-args mode; batching is by line not arg count.
# Input has two lines with 3 and 1 tokens. -n 2 -L 1 means -L 1 wins:
# invocations are (a, b, c) and (d), one per line.
cat > "$phx_test_input" <<EOF
a b c
d
EOF

cat > "$phx_expected_output" <<EOF
a b c
d
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 2 -L 1' \
  ''
