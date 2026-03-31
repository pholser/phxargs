#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# With -t -I, trace shows the fully substituted command for each invocation.
cat > "$phx_test_input" <<'EOF'
a
b
EOF

cat > "$phx_expected_output" <<'EOF'
hello a
hello b
EOF

cat > "$phx_expected_error" <<'EOF'
echo hello a
echo hello b
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-t -I {}' \
  'echo hello {}'
