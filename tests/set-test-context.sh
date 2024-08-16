#!/usr/bin/env bash

test_dir=$(dirname "$(readlink -f "$0")")
test_name=$(basename "$(readlink -f "$0")" | sed 's/^test_//')

cd "$test_dir" || exit 3

build_dir="$test_dir/../build"
test_output_dir="$build_dir/output"
mkdir -p "$test_output_dir"

test_input=$(mktemp)
[ $? -eq 0 ] || {
  printf "error: mktemp had non-zero exit code.\n" >&2
  exit 4
}

[ -f "$test_input" ] || {
  printf "error: tempfile does not exist.\n" >&2
  exit 4
}

## set trap to remove temp file on termination, interrupt or exit
trap 'rm -f "$test_input"' SIGTERM SIGINT EXIT
