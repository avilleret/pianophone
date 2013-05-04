#!/bin/bash

PID=`pgrep pianophone`
echo $PID
if [ $PID ] ; then
	echo pianophone is already running
else
	echo start pianophone...
	cd /home/pi/pianophone
	./pianophone &
	su pi -c "pd -rt -alsa -noadc -audiooutdev 3 -blocksize 64 -r 48000 -audiobuf 10 -nogui /home/pi/pianophone/patches/abs/loader.pd" &
fi

