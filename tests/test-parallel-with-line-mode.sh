#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

cat > "$phx_test_input" <<EOF
arg1 arg2
arg3 arg4
arg5 arg6
EOF

# Each parallel invocation writes its output to a separate file to avoid
# stdout interleaving between concurrent processes.
out_dir=$(mktemp -d)
trap 'rm -rf "$out_dir"' EXIT

/usr/bin/env -i "$phx_build_dir/phxargs" \
  -P 2 -L 1 \
  sh -c 'echo "$@" > "'"$out_dir"'/$1"' -- \
  < "$phx_test_input"

actual=$(sort "$out_dir"/*)
expected=$(printf '%s\n' 'arg1 arg2' 'arg3 arg4' 'arg5 arg6' | sort)

if [ "$actual" != "$expected" ]; then
  echo "$phx_test_name: output differs" >&2
  diff <(echo "$expected") <(echo "$actual") >&2
  exit 1
fi
