#!/usr/bin/env bash
# Kills itself with SIGTERM so phxargs sees a signal-terminated child.
kill -TERM $$
