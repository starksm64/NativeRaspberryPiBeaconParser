#!/usr/bin/env bash

# Run the beacon scanner in the analyzer mode where the scanner simply reads beacon events and then
# reports the counts of events for each time window

# Absolute path to this script
SCRIPT=$(readlink -f $0)
BIN=`dirname $SCRIPT`
ROOT=`dirname $BIN`

# Aliases for backward compatibility to previous variables used from scanner.conf
scannerID=${scannerID:=$SCANNER_ID}

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
SCANNER_ID="`hostname`"
echo "Running in anylyze mode(--analyzeMode), specify a non-default time window using --analyzeWindow"
${ROOT}/Debug/src2/NativeScannerBlueZ --scannerID "${scannerID}" --analzyeMode --skipPublish $*
