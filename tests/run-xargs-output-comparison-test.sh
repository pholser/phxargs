#!/usr/bin/env bash

test_name=$1
xargs_input_producer_cmd=$2
xargs_options=$3
xargs_cmd_line=$4

test_dir=$(dirname "$(readlink -f "$0")")
cd "$test_dir" || exit 3

build_dir="$test_dir/../build"
test_output_dir="$build_dir/output"
mkdir -p "$test_output_dir"

$xargs_input_producer_cmd \
  | xargs $xargs_options $xargs_cmd_line \
  > "$test_output_dir/xargs-$test_name.out" \
  2> "$test_output_dir/xargs-$test_name.err"
$xargs_input_producer_cmd \
  | "$build_dir/phxargs" $xargs_options $xargs_cmd_line \
  > "$test_output_dir/phxargs-$test_name.out" \
  2> "$test_output_dir/phxargs-$test_name.err"

cd "$test_output_dir" || exit 3
diff "xargs-$test_name.out" "phxargs-$test_name.out"
out_comparison_failed=$?
diff "xargs-$test_name.err" "phxargs-$test_name.err"
err_comparison_failed=$?

if [ $out_comparison_failed -ne 0 ] ; then
  echo "$test_name: stdout differs" >&2
fi
if [ $err_comparison_failed -ne 0 ] ; then
  echo "$test_name: stderr differs" >&2
fi

exit $(( out_comparison_failed + err_comparison_failed ))
