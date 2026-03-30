#!/usr/bin/env bash
# Dispatches to different failure modes based on argument.
# "signal"  -> kills itself with SIGTERM (exit via signal)
# "succeed" -> exits with code 0 (success)
# anything else -> exits with code 1 (plain failure)
if [ "$1" = "signal" ]; then
  kill -TERM $$
elif [ "$1" = "succeed" ]; then
  exit 0
fi
exit 1
