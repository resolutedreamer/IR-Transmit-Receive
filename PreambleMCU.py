import thread
import sys
from time import sleep
import os.path

preamble_length_path = "preamble_length.txt"

def deliver_message(threadName, delay):
    while (True):
        try:
            # Check if the file has been written into with Ready
            with open(preamble_length_path) as f:
                for line in f:
                    
                    print "Delivering Message: " + line + '\n'
                    transmit_system_call = 'echo "' + line + '" > /dev/ttymcu0'
                    os.system(transmit_system_call)
                    
                    sleep(2)
                    
        except KeyboardInterrupt:
            print "\nBye"
            sys.exit()

def verify_message_1(threadName, delay):
    try:
        verify_system_call_0 = 'cat /dev/ttymcu0'
        os.system(verify_system_call_0)
    except:
        print "qq"

def verify_message_2(threadName, delay):
    try:
        verify_system_call_1 = 'cat /dev/ttymcu1'
        os.system(verify_system_call_1)
    except:
        print "qq"

try:
    thread.start_new_thread(deliver_message, ("OK", 2,))
    #thread.start_new_thread(verify_message_1, ("OK1", 2,))
    thread.start_new_thread(verify_message_2, ("OK2", 2,))
    
except:
    print "Error: Unable to start Thread"

while 1:
    pass