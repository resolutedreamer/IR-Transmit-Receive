/////////////////////////////////////////////////////////////////////////
//
//	pwm.c
//
//	Original Author:
//	Raymond Andrade
//
// 	Contributing Authors:
// 	Anthony Nguyen
// 	Pranjal Rastogi
//
// 	Parameters:
// 	argc and argv
// 
//	Provide a number from 0-3 to indicate the ID# 
//	of the Edison anchor node (defaults to 0) 
//
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <mraa/pwm.h>
/* File control definitions */
#include <fcntl.h> 

#define PREAMBLE_DURATION 5
char preamble_length = 5;
#define SCALING_FACTOR 5 
#define PREAMBLE_DELAY 750000/SCALING_FACTOR // 10 miliseconds

#define SHORT_DELAY 350000/SCALING_FACTOR // 5 miliseconds
#define LONG_DELAY 1400000/SCALING_FACTOR // 20 miliseconds
#define MID_DELAY 1050000/SCALING_FACTOR // 15 miliseconds

mraa_pwm_context pwm1;
mraa_pwm_context pwm2;
mraa_pwm_context pwm3;
mraa_pwm_context pwm4;

void check_preamble_length() {
	int fd;
    unsigned char data;
    unsigned char buf[100];
    int i, n;
    fd = open("./preamble_length.txt", O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("open preamble_length.txt failed!\n");
        return;
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
	preamble_length = buf[0];
 
    close(fd);

}

void send_preamble_sequence(int preamble_duration, float duty) {
	int i = 0, j = 0;
	printf("\nSending Preamble. ");

	for(i = preamble_duration; i > 0; i--){
		
		//equal durations
		mraa_pwm_write(pwm1, duty); // high
		mraa_pwm_write(pwm2, duty); // high
		mraa_pwm_write(pwm3, duty); // high
		mraa_pwm_write(pwm4, duty); // high
		for(j = PREAMBLE_DELAY; j > 0; j--);

		mraa_pwm_write(pwm1, 0); // low
		mraa_pwm_write(pwm2, 0); // low
		mraa_pwm_write(pwm3, 0); // low
		mraa_pwm_write(pwm4, 0); // low
		for(j = PREAMBLE_DELAY; j > 0; j--);
	}
}

void send_low_bit(float duty) {
	int i = 0;
	printf("0 ");
	
	mraa_pwm_write(pwm1, duty);
	mraa_pwm_write(pwm2, duty);
	mraa_pwm_write(pwm3, duty);
	mraa_pwm_write(pwm4, duty);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
	
	mraa_pwm_write(pwm1, 0);
	mraa_pwm_write(pwm2, 0);
	mraa_pwm_write(pwm3, 0);
	mraa_pwm_write(pwm4, 0);
	for(i = LONG_DELAY; i > 0; i--); // 20 ms
}

void send_high_bit(float duty) {
	int i = 0;
	printf("1 ");
	
	mraa_pwm_write(pwm1, duty);	
	mraa_pwm_write(pwm2, duty);	
	mraa_pwm_write(pwm3, duty);	
	mraa_pwm_write(pwm4, duty);	
	for(i = LONG_DELAY; i > 0; i--); //20 ms

	mraa_pwm_write(pwm1, 0);
	mraa_pwm_write(pwm2, 0);
	mraa_pwm_write(pwm3, 0);
	mraa_pwm_write(pwm4, 0);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
}

int main(int argc, char *argv[]) {
	
	int transmit_counter = 0;
	int i = 0;
	// GPIO Initialization - Edison has 4 PWM pins
	float duty = .5f;
	
	pwm1 = mraa_pwm_init(3);
	mraa_pwm_period_us(pwm1, 26);
	mraa_pwm_enable(pwm1, 1);
	
	pwm2 = mraa_pwm_init(5);
	mraa_pwm_period_us(pwm2, 26);
	mraa_pwm_enable(pwm2, 1);
	
	pwm3 = mraa_pwm_init(6);
	mraa_pwm_period_us(pwm3, 26);
	mraa_pwm_enable(pwm3, 1);
	
	pwm4 = mraa_pwm_init(9);
	mraa_pwm_period_us(pwm4, 26);
	mraa_pwm_enable(pwm4, 1);

	// by default, the edison has ID 0
	int edisonID = 0;
	// if we pass in an argument, use it for the edisonID
	// please pass in an integer
	if (argc == 2) {
		edisonID = atoi(argv[1]);
	}
	if (argc == 3) {
		edisonID = atoi(argv[1]);
		preamble_length = atoi(argv[2]);
	}
	// This will transmit IR data on all 4 pins at once
	// on a single Edison board

	
	// Continuously Transmit IR Signal when the program is running
	while(1) {
		
		check_preamble_length();
		
		// Preamble - Signals the Receiver Message Incoming
		send_preamble_sequence(preamble_length, duty);
		
		// Sending Edison Board ID # - 2 bits, MSB then LSB
		switch (edisonID) {
			case 0:
				printf("EdisonID: 0 - ");
				send_low_bit(duty);	// Send lsb bit 0 = LOW
				send_low_bit(duty);	// Send msb bit 1 = LOW
				break;
			case 1:
				printf("EdisonID: 1 - ");
				send_high_bit(duty);	// Send lsb bit 0 = HIGH
				send_low_bit(duty);	// Send msb bit 1 = LOW
				break;
			case 2:
				printf("EdisonID: 2 - ");
				send_low_bit(duty);	// Send lsb bit 0 = LOW
				send_high_bit(duty);	// Send msb bit 1 = 
				break;
			case 3:
				printf("EdisonID: 3 - ");
				send_high_bit(duty);	// Send lsb bit 0 = HIGH
				send_high_bit(duty);	// Send msb bit 1 = HIGH
				break;
			default:
				send_low_bit(duty);	// Send lsb bit 0 = LOW
				send_low_bit(duty);	// Send msb bit 1 = LOW				
		}
		
		// Sending Edison IR Emitter ID # - 2 bits, MSB then LSB
		
		// pwm1 = 00 = short-long/short-long = 5-20/5-20
		// pwm2 = 01 = short-long/long-short = 5-20/20-5
		// pwm3 = 10 = long-short/short-long = 20-5/5-20
		// pwm4 = 11 = long-short/long-short = 20-5/20-5
		
		// First Bit
		mraa_pwm_write(pwm1, duty);
		mraa_pwm_write(pwm2, duty);
		mraa_pwm_write(pwm3, duty);
		mraa_pwm_write(pwm4, duty);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		mraa_pwm_write(pwm1, 0);
		mraa_pwm_write(pwm2, 0);
		//mraa_pwm_write(pwm3, duty);
		//mraa_pwm_write(pwm4, duty);
		for(i = MID_DELAY; i > 0; i--); // 15 ms

		//mraa_pwm_write(pwm1, 0);
		//mraa_pwm_write(pwm2, 0);
		mraa_pwm_write(pwm3, 0);
		mraa_pwm_write(pwm4, 0);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		// Second Bit	
		mraa_pwm_write(pwm1, duty);	
		mraa_pwm_write(pwm2, duty);	
		mraa_pwm_write(pwm3, duty);	
		mraa_pwm_write(pwm4, duty);	
		for(i = SHORT_DELAY; i > 0; i--); // 20 ms

		mraa_pwm_write(pwm1, 0);	
		//mraa_pwm_write(pwm2, duty);	
		mraa_pwm_write(pwm3, 0);	
		//mraa_pwm_write(pwm4, duty);	
		for(i = MID_DELAY; i > 0; i--); // 15 ms

		//mraa_pwm_write(pwm1, 0);
		mraa_pwm_write(pwm2, 0);
		//mraa_pwm_write(pwm3, 0);
		mraa_pwm_write(pwm4, 0);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		/*
		send_message_bit(edisonID, 0, duty, pwm1);
		send_message_bit(edisonID, 0, duty, pwm1);
		send_message_bit(edisonID, 0, duty, pwm1);
		send_message_bit(edisonID, 0, duty, pwm1);
		
		
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 0, duty, pwm1);
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 1, duty, pwm2);			
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 2, duty, pwm3);
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0 ;transmit_counter--) {
			send_message_bit(edisonID, 3, duty, pwm4);
		}
		*/
		
	}// end while loop

	mraa_pwm_write (pwm1, 0);
	mraa_pwm_enable(pwm1, 0);
	mraa_pwm_write (pwm2, 0);
	mraa_pwm_enable(pwm2, 0);
	mraa_pwm_write (pwm3, 0);
	mraa_pwm_enable(pwm3, 0);
	mraa_pwm_write (pwm4, 0);
	mraa_pwm_enable(pwm4, 0);

	return 0;
}//end main
