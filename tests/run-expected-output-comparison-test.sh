#!/usr/bin/env bash

test_name=$1
test_input=$2
expected_output=$3
expected_error=$4
xargs_options=$5
xargs_cmd_line=$6

test_dir=$(dirname "$(readlink -f "$0")")
cd "$test_dir" || exit 3

build_dir="$test_dir/../build"
test_output_dir="$build_dir/output"
mkdir -p "$test_output_dir"

if [[ "$xargs_options" =~ "-a"[[:space:]] ]] ; then
  /usr/bin/env -i "$build_dir/phxargs" $xargs_options $xargs_cmd_line \
    > "$test_output_dir/phxargs-$test_name.out" \
    2> "$test_output_dir/phxargs-$test_name.err"
else
  /usr/bin/env -i "$build_dir/phxargs" $xargs_options $xargs_cmd_line \
    < "$test_input" \
    > "$test_output_dir/phxargs-$test_name.out" \
    2> "$test_output_dir/phxargs-$test_name.err"
fi

cd "$test_output_dir" || exit 3
diff "$expected_output" "phxargs-$test_name.out"
out_comparison_failed=$?
diff "$expected_error" "phxargs-$test_name.err"
err_comparison_failed=$?

if [ $out_comparison_failed -ne 0 ] ; then
  echo "$test_name: stdout differs" >&2
fi
if [ $err_comparison_failed -ne 0 ] ; then
  echo "$test_name: stderr differs" >&2
fi

exit $(( out_comparison_failed + err_comparison_failed ))
