#!/bin/bash
#~ first stop all...
killall pianophone pd

#~ for each partition on removable drive
for drive in  /dev/sd??* 
do
	#~ mount partition
	echo mount $drive....
	mount $drive /media/usb
	if [ -d /media/usb/pianophone-patches ]; then
	#~ if there is a pianophone-patches folder, copy it
		echo Copying files...
		if [ -d /home/pi/pianophone/patches ]; then
			rm -rf /home/pi/pianophone/patches
		fi
  		/home/pi/pianophone/lcd "Copie des fichiers..."
		cp -r /media/usb/pianophone-patches /home/pi/pianophone/patches
        	echo OK
	else
		echo Can\'t find \"pianophone-patches\" folder on usb drive
	fi
	echo -n "umount..."
	umount /media/usb
	echo OK
done

#~ print some fun on LCD
/home/pi/pianophone/lcd "Youpi c'est fini"
sleep 2
/home/pi/pianophone/lcd "Retire la cle !"

exit
