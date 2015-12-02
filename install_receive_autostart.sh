#!/bin/sh

#copy the startup script from here into system startup folder
cp ./etc/init.d/Client.sh /etc/init.d/
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