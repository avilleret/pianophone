#!/bin/bash
_ETH0=$(ifconfig | grep eth0) || true

if [ "$_ETH0" ]; then
   dhclient -r
   echo -n "1-1.1:1.0" | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind
   /home/pi/pianophone/lcd "Ethernet disable"
fi

