#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# With -t -n 1, each invocation produces its own trace line on stderr
# immediately before the command executes.
cat > "$phx_test_input" <<'EOF'
a
b
c
EOF

cat > "$phx_expected_output" <<'EOF'
a
b
c
EOF

cat > "$phx_expected_error" <<'EOF'
echo a
echo b
echo c
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-t -n 1' \
  'echo'
