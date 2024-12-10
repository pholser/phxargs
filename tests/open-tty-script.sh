#!/bin/bash

echo "From stdin: $@"

read -p "Enter input: " tty_input < /dev/tty
echo "Received: $tty_input"
