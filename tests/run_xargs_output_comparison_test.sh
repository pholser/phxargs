#!/usr/bin/env bash

test_dir=$(dirname "$(readlink -f "$0")")
cd "$test_dir" || exit 3

build_dir="$test_dir/../build"

test_name=$1
xargs_input=$2
xargs_options=$3
cmd_line=$4

xargs $xargs_options $cmd_line < "$xargs_input" > "xargs_$test_name.out" 2> "xargs_$test_name.err"
"$build_dir/phxargs" $xargs_options $cmd_line < "$xargs_input"> "phxargs_$test_name.out" 2> "phxargs_$test_name.err"
diff "xargs_$test_name.out" "phxargs_$test_name.out"
out_comparison_failed=$?
diff "xargs_$test_name.err" "phxargs_$test_name.err"
err_comparison_failed=$?

if [ $out_comparison_failed -ne 0 ] ; then
  echo "$test_name: stdout differs" >&2
fi
if [ $err_comparison_failed -ne 0 ] ; then
  echo "$test_name: stderr differs" >&2
fi

exit $(( out_comparison_failed + err_comparison_failed ))
