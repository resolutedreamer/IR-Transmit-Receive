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

if (sys.platform == "linux2"):
    current_location_file_path = "./current_location_update"
    pwm_receive_path = "./pwm_receive"
elif (sys.platform == "win32"):
    current_location_file_path = "X:/current_location_update.csv"
    pwm_receive_path = "X:/pwm_receive"
    
i = 5
while (i > 0):
    i = i - 1
    #added by Anthony
    returnValue = -1
    try:
        returnValue = subprocess.call(pwm_receive_path)
        print "pwm_receive returned: " + str(returnValue)
    except subprocess.CalledProcessError, returncode:
        print subprocess.CalledProcessError
        print "CalledProcessError Exception"
        print returncode
        returnValue = returncode
    except:
        print "Exception"
    
    ## interpret returncode as edisonID + LEDID
    #edisonID = -1
    #ledID = -1
    
    #if returncode >= 0 and returncode < 4:
    #    edisonID = 0
    #    ledID = returncode
    #elif returncode >= 4 and returncode < 8:
    #    edisonID = 1
    #    ledID = returncode - 4
    #elif returncode >= 8 and returncode < 12:
    #    edisonID = 2
    #    ledID = returncode - 8
    #elif returncode >= 12 and returncode < 16:
    #    edisonID = 3
    #    ledID = returncode - 12
    
    #print "edisonID = " + str(edisonID)
    #print "ledID = " + str(ledID)
    
    # print current_sample
    # file writing in python is in blocking mode to avoid RAW hazard by default it is blocking on Unix
    # The os.write call won't return immediately, it will sleep until the write is complete by default in Unix
    #value_to_write = str(datetime.datetime.now()) + '\t' + str(edisonID) + '\t' + str(ledID) + '\n'
    
    value_to_write = str(datetime.datetime.now()) + '\t' + str(returnValue) + '\n'
    print value_to_write
    
    with open(current_location_file_path, 'a+') as f:
        f.write(value_to_write)
        f.close()
    
    sleep(3)
    
