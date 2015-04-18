#!/bin/bash
# systemd launch script for the speakIP.pl script. This requires espeak and the sound module loaded, for example:
# /etc/sysconfig/modules/snd_bcm2835.modules
##!/bin/bash
#
# exec /sbin/modprobe snd_bcm2835

start() {
    echo "Starting speakIP..."
    /root/NativeRaspberryPiBeaconParser/bin/speakIP.pl &
}

stop() {
    echo "Stopping speakIP..."
    pids=`ps ax | grep "speakIP.pl" | awk '{print $1}'`
    if [ -z "$pids" ] ; then
       echo "speakIP is not running"
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
