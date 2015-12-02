#!/bin/sh

#compile the program
gcc -lmraa -o pwm_recieve ./source/pwm_receive.c
#copy into install location and remove extra copy
cp pwm_receive /bin/
rm pwm_receive

#copy Client.py into bin
cp ./source/Client.py /bin/

#copy config files for receiver into /etc/IR/
cp ./conf/server_ip.txt /etc/IR/server_ip.txt
cp ./conf/preamble_length.txt /etc/IR/preamble_length.txt
cp ./conf/Ready.txt /etc/IR/Ready.txt
