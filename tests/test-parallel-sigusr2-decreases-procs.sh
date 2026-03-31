#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# Send SIGUSR2 repeatedly to phxargs while it is running with -P 4.
# max_procs should floor at 1 and phxargs should still produce all output.
seq 10 > "$phx_test_input"

seq 10 > "$phx_expected_output"

cat > "$phx_expected_error" <<EOF
EOF

phx_actual_output="$phx_build_dir/output/phxargs-$phx_test_name.out"
phx_actual_error="$phx_build_dir/output/phxargs-$phx_test_name.err"
mkdir -p "$phx_build_dir/output"

/usr/bin/env -i "$phx_build_dir/phxargs" -P 4 -n 1 echo \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error" &
phxargs_pid=$!
sleep 0.1

kill -USR2 "$phxargs_pid" 2>/dev/null
kill -USR2 "$phxargs_pid" 2>/dev/null
kill -USR2 "$phxargs_pid" 2>/dev/null
kill -USR2 "$phxargs_pid" 2>/dev/null
kill -USR2 "$phxargs_pid" 2>/dev/null

wait "$phxargs_pid"
actual_exit=$?

if [ "$actual_exit" -ne 0 ]; then
  echo "$phx_test_name: expected exit 0, got $actual_exit" >&2
  exit 1
fi

diff <(sort "$phx_expected_output") <(sort "$phx_actual_output")
out_ok=$?
diff "$phx_expected_error" "$phx_actual_error"
err_ok=$?

exit $(( out_ok + err_ok ))
