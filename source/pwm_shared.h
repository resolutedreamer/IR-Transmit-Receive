#include <stdio.h>
#include <string.h>
#include <mraa/pwm.h>
/* File control definitions */
#include <fcntl.h>

int check_preamble_length() {
	int fd;
    unsigned char data;
    unsigned char buf[100];
    int i, n;
	int returnValue = -1;
    fd = open("/etc/IR/preamble_length.txt", O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("open preamble_length.txt failed!\n");
        return returnValue;
    }
 
    i = 0;
 
    do
    {
        n = read(fd, &data, 1);
        buf[i] = data;
        i++;
    } while(data != '\n' && i < 100);
 
    i = (i >=99? 99:i);
    buf[i] = '\0';
    printf("Response from preamble_length.txt: %s", buf);
	returnValue = buf[0];
 
    close(fd);
	return returnValue;
}
