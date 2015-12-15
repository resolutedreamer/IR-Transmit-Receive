#!/bin/sh
echo 48 > /sys/class/gpio/export 
echo 255 > /sys/class/gpio/export 
echo 223 > /sys/class/gpio/export 
echo 214 > /sys/class/gpio/export
#set tri state low to do work 
echo low > /sys/class/gpio/gpio214/direction 
echo low > /sys/class/gpio/gpio255/direction 
echo low > /sys/class/gpio/gpio223/direction 
echo in > /sys/class/gpio/gpio48/direction 
#set tri state high again to activate
echo high > /sys/class/gpio/gpio214/direction

