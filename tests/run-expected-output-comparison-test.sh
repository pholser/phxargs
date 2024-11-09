#!/usr/bin/env bash

phx_test_name=$1
phx_test_input=$2
phx_expected_output=$3
phx_expected_error=$4
phx_xargs_options=$5
phx_xargs_cmd_line=$6

phx_test_dir=$(dirname "$(readlink -f "$0")")
cd "$phx_test_dir" || exit 3

phx_build_dir="$phx_test_dir/../build"
phx_test_output_dir="$phx_build_dir/output"
mkdir -p "$phx_test_output_dir"

if [[ "$phx_xargs_options" =~ "-a"[[:space:]] ]]; then
  /usr/bin/env -i "$phx_build_dir/phxargs" \
    $phx_xargs_options \
    $phx_xargs_cmd_line \
    > "$phx_test_output_dir/phxargs-$phx_test_name.out" \
    2> "$phx_test_output_dir/phxargs-$phx_test_name.err" \
    < "$phx_test_input"
else
   /usr/bin/env -i "$phx_build_dir/phxargs" \
     $phx_xargs_options \
     $phx_xargs_cmd_line \
     > "$phx_test_output_dir/phxargs-$phx_test_name.out" \
     2> "$phx_test_output_dir/phxargs-$phx_test_name.err" \
     < "$phx_test_input"
fi

cd "$phx_test_output_dir" || exit 3
diff "$phx_expected_output" "phxargs-$phx_test_name.out"
out_comparison_failed=$?
diff "$phx_expected_error" "phxargs-$phx_test_name.err"
err_comparison_failed=$?

if [ $out_comparison_failed -ne 0 ] ; then
  echo "$phx_test_name: stdout differs" >&2
fi
if [ $err_comparison_failed -ne 0 ] ; then
  echo "$phx_test_name: stderr differs" >&2
fi

exit $(( out_comparison_failed + err_comparison_failed ))
