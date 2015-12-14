#!/bin/sh
my_var=$( cat /etc/IR/server_ip.txt )
python /bin/Client.py $my_var
