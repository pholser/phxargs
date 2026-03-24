#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

# Without -x, -n takes priority over -s and the size limit is not enforced.
# With -x, phxargs exits if any invocation would exceed the -s limit.
#
# Use env -i to strip the environment so that the command length overhead
# is small enough to test with a low -s value.
# "echo aaa bbb" = 5(echo) + 4(aaa) + 4(bbb) = 13 bytes > -s 12.
printf "aaa\nbbb\n" > "$phx_test_input"

phx_actual_output="$phx_test_output_dir/phxargs-$phx_test_name.out"
phx_actual_error="$phx_test_output_dir/phxargs-$phx_test_name.err"

# Without -x: batch is split to fit within -s 12; each arg in its own invocation
/usr/bin/env -i "$phx_build_dir/phxargs" \
  -n 2 -s 12 echo \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error"
actual_exit=$?

printf "aaa\nbbb\n" > "$phx_expected_output"
cat > "$phx_expected_error" <<'EOF'
EOF

if [ "$actual_exit" -ne 0 ]; then
  echo "$phx_test_name: without -x: expected exit 0, got $actual_exit" >&2
  exit 1
fi
diff "$phx_expected_output" "$phx_actual_output" || { echo "$phx_test_name: without -x: stdout differs" >&2; exit 1; }
diff "$phx_expected_error" "$phx_actual_error" || { echo "$phx_test_name: without -x: stderr differs" >&2; exit 1; }

# With -x: exits with error when batch would exceed -s 12
/usr/bin/env -i "$phx_build_dir/phxargs" \
  -x -n 2 -s 12 echo \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error"
actual_exit=$?

printf "" > "$phx_expected_output"
printf "phxargs: command too long\n" > "$phx_expected_error"

if [ "$actual_exit" -eq 0 ]; then
  echo "$phx_test_name: with -x: expected non-zero exit, got 0" >&2
  exit 1
fi
diff "$phx_expected_output" "$phx_actual_output" || { echo "$phx_test_name: with -x: stdout differs" >&2; exit 1; }
diff "$phx_expected_error" "$phx_actual_error" || { echo "$phx_test_name: with -x: stderr differs" >&2; exit 1; }
