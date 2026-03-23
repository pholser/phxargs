#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

phx_unreadable_file=$(make_temp "$phx_test_output_dir" unreadable)
chmod 000 "$phx_unreadable_file"

phx_actual_output="$phx_test_output_dir/phxargs-$phx_test_name.out"
phx_actual_error="$phx_test_output_dir/phxargs-$phx_test_name.err"

/usr/bin/env -i "$phx_build_dir/phxargs" \
  -a "$phx_unreadable_file" \
  > "$phx_actual_output" \
  2> "$phx_actual_error" \
  < "$phx_test_input"
actual_exit=$?

chmod 600 "$phx_unreadable_file"

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: cannot open arg file: Permission denied
EOF

if [ "$actual_exit" -ne 1 ]; then
  echo "$phx_test_name: expected exit 1, got $actual_exit" >&2
  exit 1
fi

diff "$phx_expected_output" "$phx_actual_output"
out_ok=$?
diff "$phx_expected_error" "$phx_actual_error"
err_ok=$?

exit $(( out_ok + err_ok ))
