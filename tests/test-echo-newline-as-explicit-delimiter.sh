#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# With -d $'\n', newline is the delimiter and spaces are literal token content.
# Each line becomes one token regardless of embedded spaces.
printf "a b\nc d\n" > "$phx_test_input"

phx_actual_output="$phx_test_output_dir/phxargs-$phx_test_name.out"
phx_actual_error="$phx_test_output_dir/phxargs-$phx_test_name.err"

/usr/bin/env -i "$phx_build_dir/phxargs" \
  -d $'\n' -n 1 echo \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error"
actual_exit=$?

printf "a b\nc d\n" > "$phx_expected_output"

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
