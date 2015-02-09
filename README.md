# NativeRaspberryPiBeaconParser
This is the native code version of the RaspberryPiBeaconParser. 
The Java version of this repository is located at: [RaspberryPiBeaconParser](https://github.com/starksm64/RaspberryPiBeaconParser), and it contains the Java org.jboss.summit2015.beacon.Beacon code for handling the creation/parsing of the MQTT binary messages produced by the NativeRaspberryPiBeaconParser.

# The iBeacon format seen in hcidump -R
TODO

# The Beacon wire format published to MQTT
The

# Creating the RaspberryPiBeaconParser pidora image
A custom pidora image needs to be created in order to be able to build the required software apis. A prebuilt custom image is available from here: [TBD]()

The preconfigured logins are:

	pi
	raspberry
	
	root
	root0FPi

Otherwsie, you can build your own image by starting with the
[Pidora-2014-R3](http://downloads.raspberrypi.org/pidora_latest) Fedora remix. See the following page for information on how to burn the image to a SD card:
[RPi_Easy_SD_Card_Setup](http://elinux.org/RPi_Easy_SD_Card_Setup)

## Customizing the base Pidora-2014-R3 image
Perform the following instalations and downloads to enable access to the development tools and code needed to build the native scanner.

	yum install net-tools
	yum install glib2-devel dbus-glib-devel gcc-c++ git

### BlueZ
	wget http://www.kernel.org/pub/linux/bluetooth/bluez-5.28.tar.xz
	tar -xJf bluez-5.28.tar.xz
	yum install libudev-devel
	yum install libical-devel readline-devel
	yum install cmake dbus-devel
	cd bluez-5.28
	./configure
	make
	make install


### Paho mqtt client
	yum install openssl-devel
	git clone https://github.com/eclipse/paho.mqtt.c.git
	cd paho.mqtt.c
	make
	make install

### activemq CMS client library
	wget http://www.apache.org/dyn/closer.cgi/activemq/activemq-cpp/3.8.3/activemq-cpp-library-3.8.3-src.tar.gz
	yum install apr-devel
	yum install cppunit cppunit-devel
	./configure
	make
	make install

### NativeRaspberryPiBeaconParser
	git clone https://github.com/starksm64/NativeRaspberryPiBeaconParser.git
	cd NativeRaspberryPiBeaconParser

The next section describes the various targets that can be built from the NativeRaspberryPiBeaconParser tree.


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
	cmake --build Debug --target NativeScannerBlueZ

## Making a release build
	mkdir Release
	cd Release
	cmake -DCMAKE_BUILD_TYPE=Release ..
	cd ..
	cmake --build Release --target NativeScanner
	cmake --build Release --target NativeScannerBlueZ

## <a name="ScanMode"></a>Putting the bluetooth interface into scan mode
	hciconfig hci0 up
	hcitool lescan --duplicates 1>/dev/null &
	hcidump -R >/tmp/hcidump.raw
	[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# hciconfig
	hci0:	Type: BR/EDR  Bus: USB
		BD Address: 5C:F3:70:64:08:BC  ACL MTU: 1021:8  SCO MTU: 64:1
		UP RUNNING 
		RX bytes:42502 acl:0 sco:0 events:1108 errors:0
		TX bytes:783 acl:0 sco:0 commands:56 errors:0

## Running an MQTT server
[~ 502]$ /usr/local/sbin/mosquitto -c /usr/local/etc/mosquitto/mosquitto.conf
1421732379: mosquitto version 1.3.5 (build date 2014-10-27 15:13:47+0000) starting
1421732379: Config loaded from /usr/local/etc/mosquitto/mosquitto.conf.
1421732379: Opening ipv4 listen socket on port 1883.
1421732379: Opening ipv6 listen socket on port 1883.

## Testing the scanner
Test parsing a stored hcidump file without publishing the messages:
[root@pidora-development-scanner NativeRaspberryPiBeaconParser]# Release/src/NativeScanner --scannerID test1 --skipPublish --rawDumpFile /tmp/hcidump.raw 

Test parsing a stored hcidump file and publish the messages:
Release/src/NativeScanner --scannerID test1 --brokerURL "tcp://192.168.1.107:1883" --rawDumpFile /tmp/hcidump.raw

## Running the scanner
Below are example command lines for running the scanner. It is assumed that previously one has put the bluetooth interface into [ScanMode](#ScanMode) on the scanner host in order for the events to be available. Note that the scanner must be run either as root, or run using sudo to allow the scanner to read the events from the hci interface.

### Run the scanner using the text output from hcidump as input
This consumes the hcidump command output as the input for the beacon events, and publishes the beacon event messages to the MQTT server running at address tcp://192.168.1.107:1883.
	
	hcidump -R | Release/src/NativeScanner --scannerID ProdScanner1 --brokerURL "tcp://192.168.1.107:1883"
	
### Run the scanner using the text file from a saved hcidump as input
This consumes the hcidump output that has been saved to a file as the input for the beacon events, and publishes the beacon event messages to the MQTT server running at address tcp://192.168.1.107:1883.
	
	Debug/src/NativeScanner --scannerID ProdScanner1 --rawDumpFile tests/hcidumpBig.raw --brokerURL "tcp://192.168.1.107:1883"
	
### Running the bluez native scanner using hci1
This command takes the beacon events coming from the hci1 bluetooth interface and publishes them to the MQTT server running at address tcp://192.168.1.107:1883.  

	Debug/src2/NativeScannerBlueZ --scannerID NativeScannerBlueZ --hciDev hci1 --brokerURL "tcp://192.168.1.107:1883"
	
### Running the bluez native scanner using hci1 and async message publishing
This command takes the beacon events coming from the hci1 bluetooth interface and publishes them to the MQTT server running at address tcp://192.168.1.107:1883 using asynchronous delivery.

	NativeScannerBlueZ --scannerID NativeScannerBlueZ --hciDev hci1 --brokerURL "tcp://192.168.1.107:1883" --asyncMode
