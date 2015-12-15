#!/bin/sh

#compile the program into the install location
gcc -lmraa -o ir_transmit ../source/ir_transmit.c
cp ir_transmit /bin/
rm ir_transmit

#create config files for transmitter into /etc/IR_conf/
cp ./conf/preamble_length.txt /etc/IR_conf/preamble_length.txt

if [ "$#" -eq 1 ]
then
	mkdir /etc/IR_conf
	touch /etc/IR_conf/edisonID.txt
	echo $1 > /etc/IR_conf/edisonID.txt
else
	cp ../etc/IR_conf/edison_id.txt /etc/IR_conf/edison_id.txt
fi

#copy the startup script from here into system startup folder
cp ../etc/init.d/ir_transmit.sh /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/ir_transmit.sh
#add to startup listing
update-rc.d ir_transmit.sh defaults

#copy the startup script from here into system startup folder
cp ../etc/init.d/report_ip.sh /etc/init.d/
cp ../etc/init.d/notify_ip.py /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/report_ip.sh
#add to startup listing
update-rc.d report_ip.sh defaults