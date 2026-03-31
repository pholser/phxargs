#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# A newline inside a quoted token is an explicit error; phxargs exits 1.
# (Contrast with unclosed quote at EOF, which is silently closed.)
printf "a 'un\nclosed'" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: unterminated quote
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  '' \
  1
