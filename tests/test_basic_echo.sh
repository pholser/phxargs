#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set_test_context.sh

"$build_dir/phxargs" \
  < usrlocal.txt \
  > "$test_output_dir/$test_name.out" \
  2> "$test_output_dir/$test_name.err"

cd "$test_output_dir" || exit 3
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

./run_xargs_output_comparison_test.sh "$test_name" usrlocal.txt "" ""
