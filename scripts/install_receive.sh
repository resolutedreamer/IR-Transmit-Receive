#!/bin/sh

#compile the program
gcc -lmraa -o ir_receive ../src/ir_receive.c
#copy into install location and remove extra copy
cp ir_receive /bin/
rm ir_receive

#copy Client.py into bin
cp ../src/Client.py /bin/

#copy config files for receiver into /etc/IR_conf/
mkdir /etc/IR_conf
cp ../etc/IR_conf/server_ip.txt /etc/IR_conf/server_ip.txt
cp ../etc/IR_conf/preamble_length.txt /etc/IR_conf/preamble_length.txt
cp ../etc/IR_conf/Ready.txt /etc/IR_conf/Ready.txt

#copy the startup script from here into system startup folder
mkdir /etc/init.d
cp ../etc/init.d/Client.sh /etc/init.d
#make the script executable
chmod 777 /etc/init.d/Client.sh
#add to startup listing
update-rc.d Client.sh defaults

#copy the startup script from here into system startup folder
cp ../etc/init.d/report_ip.sh /etc/init.d/
cp ../etc/init.d/notify_ip.py /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/report_ip.sh
#add to startup listing
update-rc.d report_ip.sh defaults