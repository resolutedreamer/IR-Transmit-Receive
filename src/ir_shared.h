/////////////////////////////////////////////////////////////////////////
//
//	ir_shared.h
//
//	Author: Anthony Nguyen
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mraa.h"
#include <mraa/pwm.h>
#include <mraa/gpio.h>
#include <sched.h>
/* File control definitions */
#include <fcntl.h>

int check_preamble_length() {
	int fd;
    unsigned char data;
	// Preamble length will be a single digit from 0-9
    unsigned char buf[2];
    int i, n, preamble_length;
	int returnValue = -1;
    fd = open("/etc/IR_conf/preamble_length.txt", O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("open preamble_length.txt failed!\n");
        return returnValue;
    }
	else {
		i = 0;		 
		do
		{
			n = read(fd, &data, 1);
			buf[i] = data;
			i++;
		} while(data != '\n' && i < 2);
	 
		i = (i >=99? 99:i);
		buf[i] = '\0';
		preamble_length = buf[0]-48;
		printf("Response from preamble_length.txt: %d\n", preamble_length);
		returnValue = preamble_length;
	 
		close(fd);
		return returnValue;	
	}
}
