#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

printf " a  \0   b    \0      c\0" > "$test_input"
printf " a      b    \n      c\n" > "$expected_output"

./run-expected-output-comparison-test.sh \
  $test_name \
  "$test_input" \
  "$expected_output" \
  "$expected_err" \
  '-0 -n 2' \
  ''
