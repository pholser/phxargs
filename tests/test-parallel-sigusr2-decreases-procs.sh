#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

# Use a slow command so phxargs is still running when the signals arrive.
# With -P 2 and sleep 0.5 per child, the pool blocks waiting for a slot after
# the first two children are forked; the signals arrive at 0.1s, well before
# any child finishes.  Three USR2 signals drive new_max below 1, exercising
# the floor-to-1 clamp in apply_signal_adjustments.
seq 4 > "$phx_test_input"
seq 4 > "$phx_expected_output"

phx_actual_output="$phx_build_dir/output/phxargs-$phx_test_name.out"
phx_actual_error="$phx_build_dir/output/phxargs-$phx_test_name.err"
mkdir -p "$phx_build_dir/output"

/usr/bin/env -i "$phx_build_dir/phxargs" -P 2 -n 1 \
  sh -c 'sleep 0.5; printf "%s\n" "$@"' sh \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error" &
phxargs_pid=$!
sleep 0.1

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
diff /dev/null "$phx_actual_error"
err_ok=$?

exit $(( out_ok + err_ok ))
