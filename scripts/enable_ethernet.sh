#!/bin/bash
/home/pi/pianophone/lcd "Stop pianophone Start Ethernet"
killall pianophone pd
echo -n "1-1.1:1.0" | sudo tee /sys/bus/usb/drivers/smsc95xx/bind
dhclient eth0
