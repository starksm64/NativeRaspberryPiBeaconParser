#!/usr/bin/env bash
# A script that copies any newer service file in ../systemd into the /etc/systemd/system directory
# and refreshes the systemd with the new versions

for service in $(ls ~/NativeRaspberryPiBeaconParser/systemd/*.service); do
    sfile=${service##*/}
    if [ $service -nt /etc/systemd/system/$sfile ]; then
        echo $service is newer that installed service
        cp $service /etc/systemd/system/
    fi
done
systemctl --system daemon-reload
