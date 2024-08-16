#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
a
b
c
EOF

./run-xargs-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  '-L 2' \
  ''
