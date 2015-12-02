#!/bin/sh

#compile the program into the install location
gcc -lmraa -o pwm_transmit ./source/pwm_transmit.c
cp pwm_transmit /bin/
rm pwm_transmit

#create config files for transmitter into /etc/IR/
cp ./conf/preamble_length.txt /etc/IR/preamble_length.txt

if [ "$#" -eq 1 ]
then
	touch /etc/IR/edisonID.txt
	echo $1 > /etc/IR/edisonID.txt
else
	cp ./conf/edison_id.txt /etc/IR/edison_id.txt
fi