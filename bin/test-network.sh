#!/usr/bin/env bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

# Include scanner configuration
. ~/scanner.conf

# A script that runs to test whether
GATEWAY=${GATEWAY:-192.168.1.1}
IFACE=${IFACE:-wlan0}

ping -I ${IFACE} -nc4 ${GATEWAY}

if [ $? != 0 ]
then
    logger -t $0 "WiFi seems down, restarting"
    /sbin/ifdown --force ${IFACE}
    sleep 10
    /sbin/ifup ${IFACE}
else
    logger -t $0 "WiFi seems up."
fi