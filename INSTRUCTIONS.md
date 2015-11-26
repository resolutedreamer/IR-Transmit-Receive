Instructions for IR Transmit/Receive for Intel Edison
======
0. Prerequisites

1. Clone this repository to your local harddrive: `git clone https://github.com/username/projectname.git`

IR Transmitter
======

2a. Run install_transmit.sh to configure the Edison for use as an IR Transmitter
3a. Typing "pwm_receive" at the command line will start the program, which will
run until the program is manually stopped. The file /bin/edisonID.txt can take as 
input a number between 0 and 3 and that Edison ID will be transmitted. The file
/bin/preamble_length.txt can take any number from 0-5 and then this will be the
number of pramble segments transmitted.
4a. Run install_transmit_autostart.sh will configure the Edison such that the
transmitter will start when the Edison turns on.

IR Receiver
======

2b. Run install_receive.sh to configure the Edison for use as an IR Receive
3b. Typing "pwm_receive" at the command line will run the program once. It
will report back the value received. The file /bin/preamble_length.txt can
take any number from 0-5 and this will be the number of preamble segments
that the receiver will search for before accepting a message.

Client/Server Interaction
======

2c. Run install_Client.sh to configure the Edison for use as a Client 
3c. Running Client.py will start communcation between the Client and
the server. The script can take up to 2 arguments. The first argument
will be the IP address of the Server. The second argument will be a
number from 0-3 which will determine which of the tags this is.

4. Profit.

Uninstallation
======
4. Run uninstall.sh to remove all these files from the Edison.