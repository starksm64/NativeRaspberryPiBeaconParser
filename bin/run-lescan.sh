#!/bin/bash

# Bring up bluetooth interface
hciconfig hci0 up

ps -eaf | grep lescan | grep -v grep >/dev/null
found=$?
# non-zero return means lescan not seen running
if [ $found -ne 0 ]; then
        hcitool lescan --duplicates >/dev/null &
else
        echo lescan already running
fi
