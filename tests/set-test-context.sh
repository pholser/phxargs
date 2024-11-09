#!/usr/bin/env bash

make_temp() {
  temp_dir="$1"
  suffix="$2"
  if mktemp --help 2>&1 | grep -q -- '-p'; then
    temp_file=$(mktemp -p "$temp_dir")
  else
    temp_file="$temp_dir/tmp.$(date +%s%N).$$.$suffix"
    touch "$temp_file"
  fi

  if [[ -z "$temp_file" ]]; then
    echo "Failed to create temporary file" >&2
    exit 4
  fi

  echo "$temp_file"
}

phx_test_dir=$(dirname "$(readlink -f "$0")")
phx_test_name=$(basename "$(readlink -f "$0")" | sed 's/^test-//')

cd "$phx_test_dir" || exit 3

phx_build_dir="$phx_test_dir/../build"
phx_test_output_dir="$phx_build_dir/output"
mkdir -p "$phx_test_output_dir"

phx_test_input=$(make_temp "$phx_test_output_dir" in || exit 4)
phx_expected_output=$(make_temp "$phx_test_output_dir" out || exit 4)
phx_expected_error=$(make_temp "$phx_test_output_dir" err || exit 4)
phx_interaction_script=$(make_temp "$phx_test_output_dir" in || exit 4)
