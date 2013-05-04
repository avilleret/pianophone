#!/bin/bash

PID=`pgrep pianophone`
echo $PID
if [ $PID ] ; then
	echo pianophone is already running
else
	echo start pianophone...
	cd /home/pi/pianophone
	/home/pi/pianophone/lcd "Start puredata"
	su pi -c "pd -rt -alsa -noadc -audiooutdev 3 -blocksize 64 -r 48000 -audiobuf 10 -nogui /home/pi/pianophone/patches/abs/loader.pd" &
        sleep 1
        /home/pi/pianophone/lcd "Start Pianophone"
        ./pianophone &
fi

