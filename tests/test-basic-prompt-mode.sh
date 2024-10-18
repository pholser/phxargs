#!/usr/bin/env expect

set script_dir [file dirname [info script]]
set test_context [exec bash -c "source $script_dir/set-test-context.sh && (set | grep '^phx_')"]
foreach line [split $test_context "\n"] {
  if {[regexp {([^=]+)=(.*)} $line -> var val]} {
    regsub -all {\\\"} $val "\"" val
    set $var $val
  }
}

spawn $test_dir/../build/phxargs -p
