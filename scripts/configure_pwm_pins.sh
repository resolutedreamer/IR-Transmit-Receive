#!/bin/sh
echo 254 > /sys/class/gpio/export 
echo 222 > /sys/class/gpio/export 
echo 214 > /sys/class/gpio/export 
#set tri state low to do work 
echo low > /sys/class/gpio/gpio214/direction 
echo high > /sys/class/gpio/gpio254/direction 
echo in > /sys/class/gpio/gpio222/direction 
echo mode1 > /sys/kernel/debug/gpio_debug/gpio182/current_pinmux 
#set tri state high again to activate
echo high > /sys/class/gpio/gpio214/direction

