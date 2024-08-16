#!/usr/bin/env bash

test_dir=$(dirname "$(readlink -f "$0")")
test_name=$(basename "$(readlink -f "$0")" | sed 's/^test_//')

cd "$test_dir" || exit 3

build_dir="$test_dir/../build"
test_output_dir="$build_dir/output"
mkdir -p "$test_output_dir"

test_input=$(mktemp -p "$test_output_dir" || exit 4)
expected_output=$(mktemp -p "$test_output_dir" || exit 4)
expected_err=$(mktemp -p "$test_output_dir" || exit 4)
