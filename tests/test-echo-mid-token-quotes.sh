#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# Quotes mid-token splice the quoted section into the surrounding token.
# a'b c'd -> one token: ab cd
# a'b'c   -> one token: abc
# a"b c"d -> one token: ab cd
cat > "$phx_test_input" <<'EOF'
a'b c'd
a'b'c
a"b c"d
EOF

cat > "$phx_expected_output" <<'EOF'
ab cd
abc
ab cd
EOF

cat > "$phx_expected_error" <<'EOF'
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '-n 1' \
  ''
