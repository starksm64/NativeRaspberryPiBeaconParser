#!/bin/bash
# systemd launch script for the testNetwork.py script.

start() {
    echo "Starting testNetwork..."
    /root/NativeRaspberryPiBeaconParser/bin/testNetwork.py &
}

stop() {
    echo "Stopping testNetwork..."
    pids=`ps ax | grep "testNetwork.py" | awk '{print $1}'`
    if [ -z "$pids" ] ; then
       echo "testNetwork is not running"
    else
        for pid in $pids; do
            echo "killing " $pid
            kill $pid
        done
    fi
}
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        sleep 1
        start
        ;;
    *) exit 1
esac
