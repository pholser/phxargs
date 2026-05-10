#!/usr/bin/env bash

source "$(cd "$(dirname "$0")" && pwd -P)"/set-test-context.sh

phx_binary="$phx_build_dir/phxargs"

# Prefer gxargs (Homebrew GNU xargs) on macOS; detect GNU on Linux.
if command -v gxargs >/dev/null 2>&1; then
  sys_xargs="$(command -v gxargs)"
  is_gnu=true
elif xargs --version 2>&1 | grep -qi 'gnu\|findutils'; then
  sys_xargs="$(command -v xargs)"
  is_gnu=true
else
  sys_xargs="$(command -v xargs)"
  is_gnu=false
fi

pass=0
fail=0
skip=0

input_file=$(mktemp)
trap 'rm -f "$input_file"' EXIT

inp() { printf '%s' "$1" > "$input_file"; }

run() {
  local desc="$1"
  shift
  local sys_out phx_out
  sys_out=$("$sys_xargs" "$@" < "$input_file" 2>/dev/null)
  phx_out=$("$phx_binary" "$@" < "$input_file" 2>/dev/null)
  if [ "$sys_out" = "$phx_out" ]; then
    pass=$((pass + 1))
  else
    echo "FAIL: $desc"
    printf '  system:  |%s|\n' "$sys_out"
    printf '  phxargs: |%s|\n' "$phx_out"
    fail=$((fail + 1))
  fi
}

gnu_run() {
  if [ "$is_gnu" = "true" ]; then
    run "$@"
  else
    skip=$((skip + 1))
  fi
}

# -- Basic whitespace tokenization --
inp 'a b c'
run "basic tokens" echo

inp $'a\nb\nc'
run "newline-separated tokens" echo

inp $'a  b\t c'
run "mixed whitespace" echo

inp '  a b  '
run "leading/trailing whitespace" echo

: > "$input_file"
run "empty input" echo

# -- -n --
inp 'a b c'
run "-n 1" -n 1 echo

inp 'a b c d'
run "-n 2" -n 2 echo

# -- -L --
printf 'a b\nc d\n' > "$input_file"
run "-L 1" -L 1 echo

printf 'a b\nc d\ne f\n' > "$input_file"
run "-L 2" -L 2 echo

# -- Quoting --
inp "'a b' c"
run "single-quoted token" echo

inp '"a b" c'
run "double-quoted token" echo

inp "''"
run "empty single-quoted token" echo

inp '""'
run "empty double-quoted token" echo

inp "a'b c'd"
run "mid-token single quote" echo

# -- Backslash escape --
inp 'a\ b'
run "escaped space" echo

# -- -0 (null delimiter) --
printf 'a\0b\0c' > "$input_file"
run "-0 basic" -0 echo

printf 'a b\0c d' > "$input_file"
run "-0 tokens with spaces" -0 echo

# -- -I (placeholder replace) --
printf 'a\nb\n' > "$input_file"
run "-I replace" -I '{}' echo 'x{}'

# -- GNU-only --
: > "$input_file"
gnu_run "-r skips empty input" -r echo nothing

inp 'a:b:c'
gnu_run "-d single-char delimiter" -d : echo

echo "$phx_test_name: $pass passed, $fail failed, $skip skipped"
[ $fail -eq 0 ]
