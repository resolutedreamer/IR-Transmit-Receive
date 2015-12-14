#!/bin/sh

#compile the program
gcc -lmraa -o pwm_receive ./source/pwm_receive_new.c
#copy into install location and remove extra copy
cp pwm_receive /bin/
rm pwm_receive

#copy Client.py into bin
cp ./source/Client.py /bin/

#copy config files for receiver into /etc/IR/
mkdir /etc/IR
cp ./conf/server_ip.txt /etc/IR/server_ip.txt
cp ./conf/preamble_length.txt /etc/IR/preamble_length.txt
cp ./conf/Ready.txt /etc/IR/Ready.txt

#copy the startup script from here into system startup folder
mkdir /etc/init.d
cp ./etc/init.d/Client.sh /etc/init.d
#make the script executable
chmod 777 /etc/init.d/Client.sh
#add to startup listing
update-rc.d Client.sh defaults

#copy the startup script from here into system startup folder
cp ./etc/init.d/report_ip.sh /etc/init.d/
cp ./etc/init.d/notify_ip.py /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/report_ip.sh
#add to startup listing
update-rc.d report_ip.sh defaults