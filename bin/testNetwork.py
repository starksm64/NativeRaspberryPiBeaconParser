#!/usr/bin/python3

import subprocess
import os
from time import sleep

env = {}
HOME = os.environ.get("HOME", "/root")
scannerConf = open(HOME+"/scanner.conf", "rt")
while True:
    in_line = scannerConf.readline()
    if not in_line:
        break
    in_line = in_line[:-1]
    key, value = in_line.split("=")
    env[key] = value
scannerConf.close()

GATEWAY = '192.168.1.1'
if 'GATEWAY' in env:
    GATEWAY = env['GATEWAY']
IFACE = 'wlan0'
if 'IFACE' in env:
    IFACE = env['IFACE']

print("Testing GATEWAY=%s, IFACE=%s" % (GATEWAY, IFACE))
while True:
    ret = subprocess.call(['/bin/ping','-I', IFACE, '-nc4', GATEWAY])
    if ret == 0:
        print("Network appears to be up")
    else:
        print("Network appears to be down, restarting...")
        ret = subprocess.call(['/sbin/ifdown', '--force', IFACE])
        ret = subprocess.call(['/sbin/ifup', IFACE])
    sleep(60)
