#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_interaction_script" <<EOF
> arg1 arg2 arg3
> arg4 arg5 arg6
> EOF
< /bin/echo arg1 arg2 arg3 arg4 arg5 arg6...?
> y
< arg1 arg2 arg3 arg4 arg5 arg6
EOF

./run-expected-output-comparison-test.sh \
  $phx_test_name \
  "$phx_interaction_script" \
  '-p' \
  ''
