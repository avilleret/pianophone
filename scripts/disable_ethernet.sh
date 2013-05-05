#!/bin/bash
/home/pi/pianophone/lcd "Disable ethernet" "Start pianophone"
killall pd pianophone

_ETH0=$(ifconfig | grep eth0) || true

if [ "$_ETH0" ]; then
	dhclient -r
   echo -n "1-1.1:1.0" | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind
fi

/home/pi/pianophone/scripts/start.sh
