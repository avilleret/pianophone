#!/bin/bash

echo Installation de bcm2835...
cd ~

echo Téléchargement de l\'archive...
wget http://www.open.com.au/mikem/bcm2835/bcm2835-1.19.tar.gz

echo Décompression de l\'archive
tar zxvf bcm2835-1.19.tar.gz
rm bcm2835-1.19.tar.gz

echo Compilation et installation...
cd bcm2835-1.19
./configure
make
sudo make check
sudo make install

sudo apt-get install liblo-dev	
