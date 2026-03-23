#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# -d ' ' splits on literal space only; tabs and newlines are token content.
# Input "a b<TAB>c" produces two tokens: "a" and "b<TAB>c".
# echo receives both in one invocation and prints "a b<TAB>c".
printf "a b\tc" > "$phx_test_input"

phx_actual_output="$phx_test_output_dir/phxargs-$phx_test_name.out"
phx_actual_error="$phx_test_output_dir/phxargs-$phx_test_name.err"

/usr/bin/env -i "$phx_build_dir/phxargs" \
  -d ' ' \
  > "$phx_actual_output" \
  2> "$phx_actual_error" \
  < "$phx_test_input"
actual_exit=$?

printf "a b\tc\n" > "$phx_expected_output"

cat > "$phx_expected_error" <<EOF
EOF

if [ "$actual_exit" -ne 0 ]; then
  echo "$phx_test_name: expected exit 0, got $actual_exit" >&2
  exit 1
fi

diff "$phx_expected_output" "$phx_actual_output"
out_ok=$?
diff "$phx_expected_error" "$phx_actual_error"
err_ok=$?

exit $(( out_ok + err_ok ))
