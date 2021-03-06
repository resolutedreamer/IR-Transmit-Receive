## Client.py
# Usage:
# Call this file as follows:
# python Client.py <serverIP> <tagid>
# for example: python Client.py '192.168.1.2' '1'
#
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
import mraa
import pyupm_buzzer as upmBuzzer  # Available on the edison as library
# from termcolor import colored
import subprocess

# ------Globals-------- #
# List of possible location
sample_list = ["0", # 0 for debugging the case when the IR receiver receives nothing.
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
               "15",
               "16"]

# These paths are on the client
current_location_file_path = "/etc/IR_conf/current_location.csv"
ready_file_path = "/etc/IR_conf/Ready.txt"
ready_message = "ready t" # Change this based on the tag

protected_zone_breach_file_path = "/etc/IR_conf/protected_zone_breach.txt"

preamble_length_path = "/etc/IR_conf/preamble_length.txt"
ir_receive_path = "/bin/ir_receive"

print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

# Change this based on your place. However you can give it as an argument in the script call
server_IP = '192.168.1.2'

# Periodic delay for sampling
delay = 0.2
# 2 seconds for debug


try:
    server_IP = sys.argv[1]
    print "server ip passed in was " + server_IP
except:
    print "no server ip passed in, use default"

try:
    ready_message = ready_message + sys.argv[2]
    print "ready message for this tag will be " + ready_message
    protected_zone_breach_file_path = 't'+sys.argv[2]+'_protected_zone_breach.txt'
except:
    ready_message = ready_message + "1"
    print "no ready message passed in, use default 1"


print "ir_receive_path is: " + ir_receive_path
print "current_location_file_path is: " + current_location_file_path


class IR_Receive(threading.Thread):
    def __init__(self):
        # Create the thread
        threading.Thread.__init__(self)
        return

    def run(self):
        pastValue = 0 # a variable maintain the a one sample older value of the location
        while True:
            returnValue = -1
            preamble_length = ""

            try:
                # Check if the file has been written into with Ready
                with open(preamble_length_path) as f:
                    for line in f:
                        print "Preamble Length for this message: " + line.rstrip()
                        preamble_length += line.rstrip()
            except:
                print "\nNo preamble_length.txt file, assume default preamble_length = 5"

            ir_receive_call = [ir_receive_path]
            if len(preamble_length) > 0:
                ir_receive_call = [ir_receive_path, preamble_length]

            try:
                print "Calling ir_receive subprocess: " + str(ir_receive_call)
                returnValue = subprocess.call(ir_receive_call)
                print '##########################################'+ str(returnValue)
                print "ir_receive returned: " + str(returnValue)
            except subprocess.CalledProcessError, returnValue:
                print subprocess.CalledProcessError
                print "CalledProcessError Exception"
                print returnValue
            except:
                print "Exception, probably no ir_receive installed"

            # file writing in python is in blocking mode to avoid RAW hazard by default it is blocking on Unix
            # The os.write call won't return immediately, it will sleep until the write is complete by default in Unix

            # only send to the server when there is meaningful data
            # meaningful data is not zero and not repetitive value of location
            # This to solve the problem of delay of server communication that takes approximately 600 ms
            if returnValue != 0 and returnValue != pastValue:
                # update the pastValue with teh currentValue of location
                pastValue = returnValue

                value_to_write = str(datetime.datetime.now()) + '\t' + str(returnValue) + '\n'
                print value_to_write

                with open(current_location_file_path, 'a+') as f:
                    f.write(value_to_write)
                    f.close()

                # write ready in the file
                with open(ready_file_path, 'w') as f:
                    f.write(ready_message)
                    f.close()

            sleep(delay)


class TestRandomLocationGenerator(threading.Thread):
    def __init__(self):
        # Create the thread
        threading.Thread.__init__(self)
        return

    def run(self):
        pastValue = '0'
        while True:
            # get a random selection from the list
            current_sample = random.choice(sample_list)
            # print current_sample
            # file writing in python is in blocking mode to avoid RAW hazard by default it is blocking on Unix
            # The os.write call won't return immediately, it will sleep until the write is complete by default in Unix
            value_to_write = str(datetime.datetime.now()) + '\t' + current_sample + '\n'
            print value_to_write

            if current_sample != '0' and current_sample != pastValue:
                with open(current_location_file_path, 'a+') as f:
                    f.write(value_to_write)
                    f.close()

                # write ready in the file
                with open(ready_file_path, 'w') as f:
                    f.write(ready_message)
                    f.close()

                pastValue = current_sample
            sleep(delay)  # every 2 seconds for debugging now


class PollingReady(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        return

    def run(self):
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

                sleep(delay - delay/2) # decrease the delay by half in polling the ready file
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


class ProtectedZoneHazardHandle(threading.Thread):
    chords = [upmBuzzer.DO, upmBuzzer.RE, upmBuzzer.MI, upmBuzzer.FA,
              upmBuzzer.SOL, upmBuzzer.LA, upmBuzzer.SI, upmBuzzer.DO,
              upmBuzzer.SI]

    def __init__(self):
        threading.Thread.__init__(self)

    def buzz(self):
        BUZZER_PIN = 5
        # print (mraa.getVersion())
        # buzzer = mraa.Gpio(BUZZER_PIN)  # Digital
        buzzer = upmBuzzer.Buzzer(BUZZER_PIN)
        # buzzer.dir(mraa.DIR_OUT)
        buzzer.setVolume(0.25)
        for chord_ind in range(0, 2):
            # play each note for 0.1 second
            buzzer.playSound(self.chords[chord_ind], 100000)
            sleep(0.1)

    def run(self):
        # Check if the file protected zone hazard is located and updated
        while True:
            # print 'protected zone checking!'
            hazard = os.path.exists(protected_zone_breach_file_path)
            if hazard:
                print 'hazard detects --> BEEEEEP'
                self.buzz()
                # remove the file for the next breach
                os.remove(protected_zone_breach_file_path)
            sleep(delay)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)s: %(message)s',
                        )

    ip = server_IP
    port = 12345
    #TestRandomLocationGenerator().start()
    IR_Receive().start()
    PollingReady().start()
    ProtectedZoneHazardHandle().start()
