#!/bin/bash
/home/pi/pianophone/lcd "Start Ethernet"
_ETH0=$(ifconfig | grep eth0) || true

if [ "$_ETH0" ]; then
	echo eth0 found
else
	echo -n "1-1.1:1.0" | sudo tee /sys/bus/usb/drivers/smsc95xx/bind
   /etc/init.d/networking start
   dhclient eth0
   /home/pi/pianophone/lcd "Ethernet enable"
fi
