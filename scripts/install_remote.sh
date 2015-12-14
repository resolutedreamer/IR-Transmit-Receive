#!/bin/sh


if [ "$#" -eq 3 ]
then
	#$1 is ip
	#$2 is username
	#$3 is password
	#$4 is 'tx' if tag, 'ax' if anchor, x is a number
fi


scp root@MachineB ./IR_transmit_receive

ssh root@MachineB 'bash -s' < local_script.sh
