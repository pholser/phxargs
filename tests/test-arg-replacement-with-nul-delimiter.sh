#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

printf "foo\0bar\0baz\0" > "$phx_test_input"

cat > "$phx_expected_output" <<EOF
got: foo
got: bar
got: baz
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-I {} -0' \
  'echo got: {}'
