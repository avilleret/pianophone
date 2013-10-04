#!/bin/bash

PID=`pgrep pianophone`
echo $PID
if [ $PID ] ; then
	echo pianophone is already running
else
	echo start pianophone...
	cd /home/pi/pianophone
        /home/pi/pianophone/lcd "Start Pianophone"
        ./pianophone &
	/home/pi/pianophone/lcd "Start puredata"
        sleep 1
#	su pi -c "pd -nogui -alsa -rt -audiobuf 10 -blocksize 64 -audiodev 3 -r 44100 -nomidi /home/pi/pianophone/patches/abs/loader.pd" &
	#su pi -c "pd -nogui -alsa -audiobuf 10 -blocksize 64 -audiodev 3 -r 48000 -nomidi /home/pi/pianophone/patches/00_test.pd" &
fi

