#!/usr/bin/env bash

# Run the beacon scanner in the battery test mode where the scanner simply reads the heartbeat beacn events and sends
# the raw unaveraged readings to a rawHeartbeatEvents queue

# Absolute path to this script
SCRIPT=$(readlink -f $0)
BIN=`dirname $SCRIPT`
ROOT=`dirname $BIN`

# Source the scanner configuration
# Typically this should have:
# - scannerID ; the name by which the scanner is defined in the beacon event messages
# - heartbeatUUID ; the uuid of the beacon used as the heartbeat for the scanner
# - brokerURL ; the url of the activemq broker
. ~/scanner.conf

# Aliases for backward compatibility to previous variables used from scanner.conf
SCANNER_ID="`hostname`"
scannerID=${scannerID:=$SCANNER_ID}
heartbeatUUID=${HEARTBEAT_UUID:=$heartbeatUUID}

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
echo "Running in battery test mode(--batteryTestMode), to rawHeartbeatEvents queue"
ARGS="--scannerID ${scannerID}"
ARGS="${ARGS} --heartbeatUUID ${heartbeatUUID}"
ARGS="${ARGS} --batteryTestMode --destinationName rawHeartbeatEvents --useQueues --statusInterval 0"
${ROOT}/Debug/src2/NativeScannerBlueZ  ${ARGS} $*
