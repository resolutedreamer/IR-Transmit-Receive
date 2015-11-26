#!/bin/sh
cp ./etc/init.d/pwm_transmit.sh /etc/init.d/
chmod 777 /etc/init.d/pwm_transmit.sh
update-rc.d pwm_transmit.sh defaults