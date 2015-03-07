#!/bin/bash

# Absolute path to this script
SCRIPT=$(readlink -f $0)
BIN=`dirname $SCRIPT`
ROOT=`dirname $BIN`

# Source the scanner configuration
. ~/scanner.conf

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

# Start the scanner
${ROOT}/Debug/src2/NativeScannerBlueZ --scannerID "${SCANNER_ID:-`hostname`}" --brokerURL "${BROKER_URL:-192.168.1.107:5672}" --heartbeatUUID "${HEARTBEAT_UUID}" --useQueues

