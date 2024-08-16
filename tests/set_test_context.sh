#!/usr/bin/env bash

test_dir=$(dirname "$(readlink -f "$0")")
test_name=$(basename "$(readlink -f "$0")" | sed 's/^test_//')

cd "$test_dir" || exit 3
