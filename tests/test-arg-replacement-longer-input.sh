#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Long input with replacements
cat > "$phx_test_input" <<EOF
$(printf "item%.0s\n" {1..100})
EOF

cat > "$phx_expected_output" <<EOF
$(printf "Processed item%.0s\n" {1..100})
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-I {} echo Processed {}' \
  ''
