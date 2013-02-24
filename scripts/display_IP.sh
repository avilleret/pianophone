#!/bin/sh

# Print the IP address
_IP=$(hostname -I) || true
  printf "My IP address is %s\n" "$_IP"
  /home/pi/pianophone/lcd $_IP $(hostname)
