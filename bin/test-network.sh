#!/usr/bin/env bash

# Include scanner configuration
. ~/scanner.conf

# A script that runs to test whether
GATEWAY=${GATEWAY:-192.168.1.1}
IFACE=${IFACE:-wlan0}

ping -I ${IFACE} -nc4 ${GATEWAY}

if [ $? != 0 ]
then
    logger -t $0 "WiFi seems down, restarting"
    sudo /sbin/ifdown --force ${IFACE}
    sleep 10
    sudo /sbin/ifup ${IFACE}
else
    logger -t $0 "WiFi seems up."
fi