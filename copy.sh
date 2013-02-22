#!/bin/bash

date > ~/log

for drive in  /dev/sd??* 
do
	echo mount $drive....
	mount $drive /media/usb
	if [ -d /media/usb/pianophone-patches ]; then
		echo Copying files...
		if [ -d /home/pi/pianophone/patches ]; then
			rm -rf /home/pi/pianophone/patches
		fi
		cp -r /media/usb/pianophone-patches /home/pi/pianophone/patches
		echo OK
	else
		echo Can\'t find \"pianophone-patches\" folder on usb drive
	fi
	echo -n "umount..."
	umount /media/usb
	echo OK
done

# umount /media/usb

exit
