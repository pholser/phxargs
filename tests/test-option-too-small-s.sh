#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: -s 0: too small
EOF

/usr/bin/env -i ./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-s 0 -r' \
  '' \
  1
