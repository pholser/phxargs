#!/usr/bin/env bash
# Echoes its argument, but exits non-zero (without output) if the argument is "two".
[ "$1" != two ] && echo "$1"
