#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# An unclosed quote that begins mid-token is an error, whether it is
# terminated by a newline or by EOF.

cat > "$phx_expected_output" <<'EOF'
EOF

cat > "$phx_expected_error" <<'EOF'
phxargs: unterminated quote
EOF

# Case 1: unclosed mid-token single quote terminated by newline
printf "a'unclosed\n" > "$phx_test_input"
./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  '' \
  1 || exit $?

# Case 2: unclosed mid-token single quote at EOF (no newline)
printf "a'unclosed" > "$phx_test_input"
./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  '' \
  1 || exit $?

# Case 3: unclosed mid-token double quote terminated by newline
printf 'a"unclosed\n' > "$phx_test_input"
./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_test_input" \
  "$phx_expected_output" \
  "$phx_expected_error" \
  '' \
  '' \
  1
