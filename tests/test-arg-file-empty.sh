#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

phx_empty_file=$(make_temp "$phx_test_output_dir" empty)

cat > "$phx_expected_output" <<EOF

EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  "-a $phx_empty_file" \
  'echo'
