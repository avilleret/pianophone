#!/bin/bash

PID=`pgrep pianophone`
echo $PID
if [ $PID ] ; then
	echo pianophone is already running
else
	echo start pianophone...
	cd /home/pi/pianophone
	./pianophone &
	su pi -c "pd -nogui /home/pi/pianophone/patches/abs/loader.pd" &
fi

