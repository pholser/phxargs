#!/usr/bin/env bash
# Dispatches to different failure modes based on argument.
# "signal" -> kills itself with SIGTERM (exit via signal)
# anything else -> exits with code 1 (plain failure)
if [ "$1" = "signal" ]; then
  kill -TERM $$
fi
exit 1
