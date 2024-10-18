#!/usr/bin/env bash

phx_test_dir=$(dirname "$(readlink -f "$0")")
phx_test_name=$(basename "$(readlink -f "$0")" | sed 's/^test_//')

cd "$phx_test_dir" || exit 3

phx_build_dir="$phx_test_dir/../build"
phx_test_output_dir="$phx_build_dir/output"
mkdir -p "$phx_test_output_dir"

phx_test_input=$(mktemp -p "$phx_test_output_dir" || exit 4)
phx_expected_output=$(mktemp -p "$phx_test_output_dir" || exit 4)
phx_expected_error=$(mktemp -p "$phx_test_output_dir" || exit 4)
