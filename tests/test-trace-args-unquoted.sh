#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Trace output is informational only: arguments are printed bare without
# quoting, even when they contain spaces or other shell metacharacters.
cat > "$phx_test_input" <<'EOF'
'a b'
c
EOF

cat > "$phx_expected_output" <<'EOF'
a b
c
EOF

cat > "$phx_expected_error" <<'EOF'
echo a b
echo c
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-t -n 1' \
  'echo'
