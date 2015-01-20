# NativeRaspberryPiBeaconParser
Native code version of the RaspberryPiBeaconParser

# The iBeacon format seen in hcidump -R
TODO

# Building the code on pidora

## Making a debug build
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cd ..
cmake --build Debug --target NativeScanner

## Making a release build
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
cmake --build Release --target NativeScanner

## Testing
[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# hciconfig
hci0:	Type: BR/EDR  Bus: USB
	BD Address: 5C:F3:70:64:08:BC  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING 
	RX bytes:42502 acl:0 sco:0 events:1108 errors:0
	TX bytes:783 acl:0 sco:0 commands:56 errors:0

hcitool lescan --duplicates 1>/dev/null &

hcidump -R >/tmp/hcidump.raw

[~ 502]$ /usr/local/sbin/mosquitto -c /usr/local/etc/mosquitto/mosquitto.conf
1421732379: mosquitto version 1.3.5 (build date 2014-10-27 15:13:47+0000) starting
1421732379: Config loaded from /usr/local/etc/mosquitto/mosquitto.conf.
1421732379: Opening ipv4 listen socket on port 1883.
1421732379: Opening ipv6 listen socket on port 1883.

[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# Release/src/NativeScanner --scannerID test1 --skipPublish --rawDumpFile /tmp/hcidump.raw 

