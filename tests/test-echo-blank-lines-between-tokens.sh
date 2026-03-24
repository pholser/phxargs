#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Blank lines between tokens are ignored; each non-empty token is passed
# as a separate invocation under -n 1.
cat > "$phx_test_input" <<EOF
a

b

c
EOF

cat > "$phx_expected_output" <<EOF
a
b
c
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  ''
