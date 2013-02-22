#!/bin/bash

if [ -d /media/usb/pianophone-patches ]; then
	echo Copying files...
	if [ -d /home/pi/pianophone/patches ]; then
		mv /home/pi/pianophone/patches /home/pi/pianophone/patches_bak
	fi
	cp -r /media/usb/pianophone-patches /home/pi/pianophone/patches
	echo OK
else
	echo Can\'t find \"pianophone-patches\" folder on usb drive
fi

exit
