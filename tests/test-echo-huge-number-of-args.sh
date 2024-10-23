#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

for i in $(seq 1 10000); do echo -n "arg$i "; done > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
$(for i in $(seq 1 9999); do echo -n "arg$i "; done ; echo -n 'arg10000')
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  ''
