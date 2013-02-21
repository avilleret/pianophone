pianophone
==========

Turn a pianophone on a Rpi synth

installation
============

Run install_dependencies.sh to install bcm2835 and liblo.

Then :
sudo nano /etc/modprobe.d/raspi-blacklist.conf

comment the line : 
blacklist i2c-bcm2708
to allow modules to be loaded.
 Save and close the file

Then :
 sudo nano /etc/rc.local

and add this before the last exit 0 :
sudo modprobe i2c-dev #~ load i2c module
sudo chmod o+rw /dev/i2c* #~ allow acces to i2c devices

you can also add thoses this line : 
/home/pi/pianophone/pianophone & #~ start the pianophone core
su pi -c "pd /home/pi/pianophone/patches/loader.pd"

or just copy the rc.local example in this folder to /etc/
