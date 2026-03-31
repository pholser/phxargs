#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# In delimiter mode, backslash is a literal character, not an escape.
# The backslash before the delimiter is part of the token, not an escape
# of the delimiter.
printf 'a\\,b' > "$phx_test_input"

cat > "$phx_expected_output" <<'EOF'
a\
b
EOF

cat > "$phx_expected_error" <<EOF
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-d , -n 1' \
  ''
