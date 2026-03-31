#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# -r does not suppress execution when input is present, even if fewer
# lines than -L allows
printf 'a\nb\n' > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
a b
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-r -L 5' \
  'echo'
