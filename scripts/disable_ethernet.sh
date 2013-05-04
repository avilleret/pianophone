#!/bin/bash
/home/pi/pianophone/lcd "Disable ethernetStart pianophone"
dhclient -r
echo -n "1-1.1:1.0" | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind
/home/pi/pianophone/scripts/start.sh
