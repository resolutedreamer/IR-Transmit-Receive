__author__ = 'salmaelmalaki'
import asyncore
import socket
import logging
import sys
from time import sleep
import datetime
import random
import threading
from socket import error as socket_error
import errno
import os.path
#import mraa
#import pyupm_buzzer as upmBuzzer

import subprocess
# from termcolor import colored

# ------Globals-------- #
# List of possible location
sample_list = ["0",
               "1",
               "2",
               "3",
               "4",
               "5",
               "6",
               "7",
               "8",
               "9",
               "10",
               "11",
               "12",
               "13",
               "14",
               "15"]

# These paths are on the client
current_location_file_path = "current_location_update"
ready_file_path = "Ready.txt"
ready_message = "ready t2"

preamble_length_path = "preamble_length.txt"
pwm_receive_path = "./pwm_receive"

t1_protected_zone_breach_file_path = "../t1_protected_zone_breach.txt"
t2_protected_zone_breach_file_path = "../t2_protected_zone_breach.txt"

if (sys.platform == "linux2"):
    current_location_file_path = "./current_location_update"
    pwm_receive_path = "./pwm_receive"
    print "pwm_receive_path is: " + pwm_receive_path
elif (sys.platform == "win32"):
    current_location_file_path = "X:/current_location_update.csv"
    pwm_receive_path = "X:/pwm_receive"
    
    
print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

server_IP = '131.179.22.190'
try:
	server_IP = sys.argv[1]
	print "server ip passed in was " + server_IP
except:
	print "no server ip passed in, use default"

print "pwm_receive_path is: " + pwm_receive_path
print "current_location_file_path is: " + current_location_file_path
	
class PWM_Receive(threading.Thread):
    def __init__(self):
        # Create the thread and run it
        threading.Thread.__init__(self)
        # thread = Thread(target=self.generate_location_thread())
        # thread.start()
        # thread.join()
        # print "thread finished...exiting"
        return

    def run(self):
        # generate_location_thread(self):
        while True:
            returnValue = ""
            returncode = -1
            pwm_preamble_length = ""
            
            try:
                # Check if the file has been written into with Ready
                with open(preamble_length_path) as f:
                    for line in f:                        
                        print "Preamble Length for this message: " + line.rstrip()
                        pwm_preamble_length += line.rstrip()
            except:
                print "\nNo preamble_length.txt file, assume default preamble_length = 5"
            
            pwm_path = [pwm_receive_path]
            if len(pwm_preamble_length) > 0:
                pwm_path = [pwm_receive_path, pwm_preamble_length]
            
            try:
                print "Calling pwm_receive subprocess: " + str(pwm_path)
                returnValue = subprocess.call(pwm_path)
                print "pwm_receive returned: " + str(returnValue)
            except subprocess.CalledProcessError, returncode:
                print subprocess.CalledProcessError
                print "CalledProcessError Exception"
                print returncode
                returnValue = returncode
            except:
                print "Exception, probably no pwm_receive installed"

            ## interpret returncode as edisonID + LEDID
#            edisonID = -1
#            ledID = -1
#
#            if returncode >= 0 and returncode < 4:
#                edisonID = 0
#                ledID = returncode
#            elif returncode >= 4 and returncode < 8:
#                edisonID = 1
#                ledID = returncode - 4
#            elif returncode >= 8 and returncode < 12:
#                edisonID = 2
#                ledID = returncode - 8
#            elif returncode >= 12 and returncode < 16:
#                edisonID = 3
#                ledID = returncode - 12
#
#            print "edisonID = " + str(edisonID)
#            print "ledID = " + str(ledID)

            # print current_sample
            # file writing in python is in blocking mode to avoid RAW hazard by default it is blocking on Unix
            # The os.write call won't return immediately, it will sleep until the write is complete by default in Unix
            value_to_write = str(datetime.datetime.now()) + '\t' + str(returnValue) + '\n'
            print value_to_write

            with open(current_location_file_path, 'a+') as f:
                f.write(value_to_write)
                f.close()

            # write ready in the file
            with open(ready_file_path, 'w') as f:
                f.write(ready_message)
                f.close()

            sleep(0.2)


class PollingReady(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

        # logging.basicConfig(level=logging.DEBUG,
        #                     format='%(name)s: %(message)s',
        #                    )
        # Create the thread and run it
        # thread = Thread(target=self.poll_ready_message_thread())
        # thread.start()
        # thread.join()
        # print "thread finished...exiting"
        # return

    def run(self):
        # poll_ready_message_thread():
        while True:
            try:
                # Check if the file has been written into with Ready
                with open(ready_file_path) as f:
                    for line in f:
                        if 'ready' in line:
                            # self.logger.debug('Ready value is available')
                            # if ready is found send a TCP/IP socket message
                            try:
                                client = EchoClient(ip, port, message=open('Ready.txt', 'r').read())
                                asyncore.loop()
                            except socket_error as serr:
                                if serr.errno != errno.ECONNREFUSED:
                                    # Not the error we are looking for, re-raise
                                    raise serr
                                # connection refused
                                # try to reconnect
                                continue

                sleep(1)
            except KeyboardInterrupt:
                print "Bye"
                sys.exit()


class EchoClient(asyncore.dispatcher):
    # Sends messages to the server and receives responses.
    def __init__(self, host, port, message, chunk_size=512):
        self.message = message
        self.to_send = message
        self.received_data = []
        self.chunk_size = chunk_size
        self.logger = logging.getLogger('EchoClient')
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.logger.debug('connecting to %s', host)
        self.connect((host, port))
        return

    def handle_connect(self):
        self.logger.debug('handle_connect()')

    def handle_close(self):
        self.logger.debug('handle_close()')
        self.close()
        # received_message = ''.join(self.received_data)
        # if received_message == self.message:
        #     self.logger.debug('RECEIVED COPY OF MESSAGE')
        # else:
        #     self.logger.debug('ERROR IN TRANSMISSION')
        #     self.logger.debug('EXPECTED "%s"', self.message)
        #     self.logger.debug('RECEIVED "%s"', received_message)
        # return

    def writable(self):
        self.logger.debug('writable() -> %s', bool(self.to_send))
        return bool(self.to_send)

    def handle_write(self):
        sent = self.send(self.to_send[:self.chunk_size])
        self.logger.debug('handle_write() -> (%d) "%s"', sent, self.to_send[:sent])
        self.to_send = self.to_send[sent:]

    def handle_read(self):
        data = self.recv(self.chunk_size)
        self.logger.debug('handle_read() -> (%d) "%s"', len(data), data)
        self.received_data.append(data)
        # I moved the following part here instead of at handle close
        received_message = ''.join(self.received_data)
        if received_message == self.message:
            self.logger.debug('RECEIVED COPY OF MESSAGE')
            # this means that the server has already got the file
            # Clean the ready file and the location file as well
            open(ready_file_path, 'w').close()
            open(current_location_file_path, 'w').close()
        else:
            self.logger.debug('ERROR IN TRANSMISSION')
            self.logger.debug('EXPECTED "%s"', self.message)
            self.logger.debug('RECEIVED "%s"', received_message)


# class ProtectedZoneHazardHandle(threading.Thread):
#     chords = [upmBuzzer.DO, upmBuzzer.RE, upmBuzzer.MI, upmBuzzer.FA,
#               upmBuzzer.SOL, upmBuzzer.LA, upmBuzzer.SI, upmBuzzer.DO,
#               upmBuzzer.SI]
#
#     def __init__(self):
#         threading.Thread.__init__(self)
#
#     def buzz(self):
#         BUZZER_PIN = 5  # A0
#         #print (mraa.getVersion())
#         #buzzer = mraa.Gpio(BUZZER_PIN)  # Digital
#         buzzer = upmBuzzer.Buzzer(BUZZER_PIN)
#         #buzzer.dir(mraa.DIR_OUT)
#         buzzer.setVolume(0.25)
#         for chord_ind in range(0, 2):
#             # play each note for 0.1 second
#             buzzer.playSound(self.chords[chord_ind], 100000)
#             sleep(0.1)
#
#     def run(self):
#         # Check if the file protected zone hazard is located and updated
#         while True:
#             # print 'protected zone checking!'
#             hazard = os.path.exists(t2_protected_zone_breach_file_path)
#             if hazard:
#                 print 'hazard detects --> BEEEEEP'
#                 self.buzz()
#                 # remove the file for the next breach
#                 os.remove(t2_protected_zone_breach_file_path)
#             # check for the hazard file every 2 seconds
#             sleep(2)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)s: %(message)s',
                        )

    # address = ('localhost', 0)  # let the kernel give us a port
    # server = EchoServer(address)
    # ip, port = server.address  # find out what port we were given

    #ip = '172.17.100.218'  # the server ip #salma
    #ip = '131.179.22.190' #ray
    ip = server_IP
    port = 12345
    PWM_Receive().start()
    PollingReady().start()
    #ProtectedZoneHazardHandle().start()

    # client = EchoClient(ip, port, message=open('Ready.txt', 'r').read())
    # asyncore.loop()
