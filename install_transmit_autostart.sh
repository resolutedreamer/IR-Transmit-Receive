#!/bin/sh

#copy the startup script from here into system startup folder
cp ./etc/init.d/pwm_transmit.sh /etc/init.d/
#make the script executable
chmod 777 /etc/init.d/pwm_transmit.sh
#add to startup listing
update-rc.d pwm_transmit.sh defaults