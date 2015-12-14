#!/bin/sh

#compile the program into the install location
gcc -lmraa -o pwm_transmit ./source/pwm_transmit.c
cp pwm_transmit /bin/
rm pwm_transmit

#create config files for transmitter into /etc/IR/
cp ./conf/preamble_length.txt /etc/IR/preamble_length.txt

if [ "$#" -eq 1 ]
then
	mkdir /etc/IR
	touch /etc/IR/edisonID.txt
	echo $1 > /etc/IR/edisonID.txt
else
	cp ./conf/edison_id.txt /etc/IR/edison_id.txt
fi

#copy the startup script from here into system startup folder
cp ./etc/init.d/pwm_transmit.sh /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/pwm_transmit.sh
#add to startup listing
update-rc.d pwm_transmit.sh defaults

#copy the startup script from here into system startup folder
cp ./etc/init.d/report_ip.sh /etc/init.d/
cp ./etc/init.d/notify_ip.py /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/report_ip.sh
#add to startup listing
update-rc.d report_ip.sh defaults