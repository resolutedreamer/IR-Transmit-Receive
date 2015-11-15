__author__ = 'salmaelmalaki'
import asyncore
import socket
import logging
import sys
from time import sleep
import datetime
import threading
from socket import error as socket_error
import errno

#added by anthony
import subprocess

# ------Globals-------- #
# List of possible location
sample_list = ["l1",
               "l2",
               "l3",
               "l4",
               "l5",
               "l6",
               "l7",
               "l8"]

current_location_file_path = "current_location_update"
ready_file_path = "Ready.txt"
ready_message = "ready t2"


class PWM_Receive(threading.Thread):
    def __init__(self):
        # Create the thread and run it
        threading.Thread.__init__(self)
        #thread = Thread(target=self.generate_location_thread())
        #thread.start()
        #thread.join()
        #print "thread finished...exiting"
        return

    def run(self):
        #generate_location_thread(self):
        while True:
            #added by Anthony
            returncode = -1
            try:
                returncode = subprocess.call("X:/pwm_receive")
                print "Return Code Was 0. This is bad."
            except subprocess.CalledProcessError:
                print "Exception"

            ## interpret returncode as edisonID + LEDID
            edisonID = -1
            ledID = -1

            if returncode >= 0 and returncode < 4:
                edisonID = 0
                ledID = returncode
            elif returncode >= 4 and returncode < 8:
                edisonID = 1
                ledID = returncode - 4
            elif returncode >= 8 and returncode < 12:
                edisonID = 2
                ledID = returncode - 8
            elif returncode >= 12 and returncode < 16:
                edisonID = 3
                ledID = returncode - 12

            print "edisonID = " + str(edisonID)
            print "ledID = " + str(ledID)
            # print current_sample
            # file writing in python is in blocking mode to avoid RAW hazard by default it is blocking on Unix
            # The os.write call won't return immediately, it will sleep until the write is complete by default in Unix
            value_to_write = str(datetime.datetime.now()) + '\t' + str(edisonID) + '\t' + str(ledID) + '\n'
            print value_to_write

        with open(current_location_file_path, 'a+') as f:
            f.write(value_to_write)
            f.close()

        # write ready in the file
        with open(ready_file_path, 'w+') as f:
            f.write(ready_message)
            f.close()

        sleep(10)


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
        #poll_ready_message_thread():
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


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)s: %(message)s',
                        )

    # address = ('localhost', 0)  # let the kernel give us a port
    # server = EchoServer(address)
    # ip, port = server.address  # find out what port we were given

    ip = '172.17.100.218'  # the server ip
    port = 12345
    PWM_Receive().start()
    PollingReady().start()


    # client = EchoClient(ip, port, message=open('Ready.txt', 'r').read())
    # asyncore.loop()
