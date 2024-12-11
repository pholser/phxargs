#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
a  w r   t   ff
sdfsd s
EOF

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: $phx_test_dir/unexecutable: Permission denied
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  "$phx_test_dir/unexecutable" \
  126
