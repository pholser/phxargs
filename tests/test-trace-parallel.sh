#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

seq 4 > "$phx_test_input"

phx_actual_output="$phx_build_dir/output/phxargs-$phx_test_name.out"
phx_actual_error="$phx_build_dir/output/phxargs-$phx_test_name.err"
mkdir -p "$phx_build_dir/output"

/usr/bin/env -i "$phx_build_dir/phxargs" -t -P 2 -n 1 echo \
  < "$phx_test_input" \
  > "$phx_actual_output" \
  2> "$phx_actual_error"

cat > "$phx_expected_output" <<EOF
1
2
3
4
EOF

cat > "$phx_expected_error" <<EOF
echo 1
echo 2
echo 3
echo 4
EOF

diff <(sort "$phx_expected_output") <(sort "$phx_actual_output")
out_ok=$?
diff <(sort "$phx_expected_error") <(sort "$phx_actual_error")
err_ok=$?

if [ $out_ok -ne 0 ]; then
  echo "$phx_test_name: stdout differs" >&2
fi
if [ $err_ok -ne 0 ]; then
  echo "$phx_test_name: stderr differs" >&2
fi

exit $(( out_ok + err_ok ))
