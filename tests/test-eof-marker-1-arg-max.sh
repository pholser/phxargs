#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$test_input" <<EOF
  a  fe  ff we
  gg ___ asdf
  wew r
EOF

cat > "$expected_output" <<EOF
a
fe
ff
we
gg
EOF

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_err" \
  '-n 1 -E ___' \
  ''
