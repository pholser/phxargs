#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set_test_context.sh

./run_xargs_output_comparison_test.sh "$test_name" usrlocal.txt "" ""
