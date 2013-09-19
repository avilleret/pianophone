#!/bin/sh

_ID=`aplay -l | grep UGM96 | cut -d: -f1 | cut -d" " -f2`
echo $_ID
if [ $_ID ] 
then
  echo Set volume to 100% and input gain
  amixer -c $_ID set Line 92
  amixer -c $_ID set PCM 100%
  /home/pi/pianophone/lcd "Volume 0dB"  "Input Gain +6dB"
else
  echo no soundcard detected
fi

