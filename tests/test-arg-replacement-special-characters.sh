#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
\$VAR
\`command\`
EOF

cat > "$expected_output" <<EOF
VAR: \$VAR
VAR: \`command\`
EOF

cat > "$expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_error" \
  '-I {} echo VAR: {}' \
  ''
