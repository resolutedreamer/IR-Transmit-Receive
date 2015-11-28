__author__ = 'anthonynguyen'
import sys
import os
import mraa


print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

kill_this_process = 0
try:
	kill_this_process = sys.argv[1]
	print "Process To Kill: " + kill_this_process
	kill_system_call = "kill -9 " + kill_this_process
	os.system(kill_system_call)
	pwm1 = mraa.Pwm(3)
	pwm2 = mraa.Pwm(5)
	pwm3 = mraa.Pwm(6)
	pwm4 = mraa.Pwm(9)
	pwm1.enable(False)
	pwm2.enable(False)
	pwm3.enable(False)
	pwm4.enable(False)
	
except:
	print "Process ID Required"
	search_system_call = "ps | grep pwm"
	os.system(search_system_call)
	print "There is a high chance you want one of these processes"