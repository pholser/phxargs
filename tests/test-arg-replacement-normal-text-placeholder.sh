#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
replace1
replace2
EOF

cat > "$phx_expected_output" <<EOF
This is replace1, replacing a placeholder
This is replace2, replacing a placeholder
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-I PLACE echo This is PLACE, replacing a placeholder' \
  ''
