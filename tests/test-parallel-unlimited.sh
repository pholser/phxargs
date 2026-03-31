#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# -P 0 means unlimited parallelism: all inputs are launched concurrently
# without throttling. With -r and empty input, no command runs.
cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
EOF

/usr/bin/env -i ./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-P 0 -r' \
  '' \
  0
