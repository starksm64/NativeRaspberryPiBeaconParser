# NativeRaspberryPiBeaconParser
Native code version of the RaspberryPiBeaconParser
The Java version of this repository is located at: [RaspberryPiBeaconParser](https://github.com/starksm64/RaspberryPiBeaconParser), and
it contains the Java org.jboss.summit2015.beacon.Beacon code for handling the creation/parsing of the MQTT binary
messages produced by the RaspberryPiBeaconParser.

# The iBeacon format seen in hcidump -R
TODO

# The Beacon wire format published to MQTT
The

# Creating the RaspberryPiBeaconParser pidora image
TODO: need to create an image from the customized pidora image I have

# Building the code on pidora
The pidora image has a NativeRaspberryPiBeaconParser under root that contains the native scanner code. You can clone
this repository from:
git clone https://github.com/starksm64/NativeRaspberryPiBeaconParser.git

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

## Putting the bluetooth interface into scan mode
hciconfig hci0 up
hcitool lescan --duplicates 1>/dev/null &
hcidump -R >/tmp/hcidump.raw
[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# hciconfig
hci0:	Type: BR/EDR  Bus: USB
	BD Address: 5C:F3:70:64:08:BC  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING 
	RX bytes:42502 acl:0 sco:0 events:1108 errors:0
	TX bytes:783 acl:0 sco:0 commands:56 errors:0

[~ 502]$ /usr/local/sbin/mosquitto -c /usr/local/etc/mosquitto/mosquitto.conf
1421732379: mosquitto version 1.3.5 (build date 2014-10-27 15:13:47+0000) starting
1421732379: Config loaded from /usr/local/etc/mosquitto/mosquitto.conf.
1421732379: Opening ipv4 listen socket on port 1883.
1421732379: Opening ipv6 listen socket on port 1883.

## Testing
Test parsing a stored hcidump file without publishing the messages:
[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# Release/src/NativeScanner --scannerID test1 --skipPublish --rawDumpFile /tmp/hcidump.raw 

Test parsing a stored hcidump file and publish the messages:
Release/src/NativeScanner --scannerID test1 --brokerURL "tcp://192.168.1.107:1883" --rawDumpFile /tmp/hcidump.raw

## Running the scanner
hcidump -R | Release/src/NativeScanner --scannerID ProdScanner1 --brokerURL "tcp://192.168.1.107:1883"
