#!/usr/bin/env bash

source "$(dirname "$(readlink -f "$0")")"/set-test-context.sh

./run-xargs-output-comparison-test.sh $test_name 'echo "a b c"' "" ""
