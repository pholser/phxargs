#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# A line containing only a quoted empty token must count as a line in -L mode.
# Regression test: the closing quote previously caused an early return, so the
# trailing \n was seen in the next call with line_has_token=false, meaning
# on_line was never fired and both tokens ended up in a single invocation.
printf '""\nfoo\n' > "$phx_test_input"

cat > "$phx_expected_output" <<EOF

foo
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-L 1' \
  'echo'
