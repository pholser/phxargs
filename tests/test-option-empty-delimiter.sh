#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

phx_build_dir="$phx_test_dir/../build"
phx_test_output_dir="$phx_build_dir/output"
mkdir -p "$phx_test_output_dir"

/usr/bin/env -i "$phx_build_dir/phxargs" -d '' \
  > "$phx_test_output_dir/phxargs-$phx_test_name.out" \
  2> "$phx_test_output_dir/phxargs-$phx_test_name.err" \
  < /dev/null
actual_exit=$?

cat > "$phx_expected_output" <<EOF
EOF

cat > "$phx_expected_error" <<EOF
phxargs: invalid delimiter for -d
EOF

failed=0

if [[ "$actual_exit" != "1" ]]; then
  echo "$phx_test_name: expected exit 1, got $actual_exit" >&2
  failed=1
fi

diff "$phx_expected_output" "$phx_test_output_dir/phxargs-$phx_test_name.out" \
  || { echo "$phx_test_name: stdout differs" >&2; failed=1; }
diff "$phx_expected_error" "$phx_test_output_dir/phxargs-$phx_test_name.err" \
  || { echo "$phx_test_name: stderr differs" >&2; failed=1; }

exit $failed
