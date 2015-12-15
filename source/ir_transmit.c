/////////////////////////////////////////////////////////////////////////
//
//	ir_transmit.c
//
//	Authors: Raymond Andrade, Anthony Nguyen, Pranjal Rastogi
//
// 	Parameters:
// 	argc and argv
// 
//	Argument 1: preamble_length
//	Provide a number from 0-5 to indicate the 
//	number of preamble sequences that the
// 	transmitter is expeceted to send (defaults to 5)
//
//	Argument 2: edisonID
//	Provide a number from 0-3 to indicate the ID# 
//	of the Edison anchor node (defaults to 0) 
//
/////////////////////////////////////////////////////////////////////////
#include "ir_shared.h"

int preamble_length = 5;
#define SCALING_FACTOR 5 
#define PREAMBLE_DELAY 750000/SCALING_FACTOR // 10 miliseconds

// by default, the edison has ID 0
int edisonID = 0;

#define SHORT_DELAY 350000/SCALING_FACTOR // 5 miliseconds
#define LONG_DELAY 1400000/SCALING_FACTOR // 20 miliseconds
#define MID_DELAY 1050000/SCALING_FACTOR // 15 miliseconds

#define LOW 0
#define HIGH 1
#define DUTY .5f

mraa_pwm_context pwm1;
mraa_pwm_context pwm2;
mraa_pwm_context pwm3;
mraa_pwm_context pwm4;

// to invert, swap DUTY and 0 here
/*
void WRITE_HIGH(mraa_pwm_context pwm) {
	mraa_pwm_write(pwm, DUTY);
}

void WRITE_LOW(mraa_pwm_context pwm) {
	mraa_pwm_write(pwm, 0);
}
*/

void send_preamble_sequence(int preamble_length) {
	int i = 0, j = 0;
	
	for(i = preamble_length; i > 0; i--){
		
		//equal durations
		mraa_pwm_write(pwm1,DUTY);
		mraa_pwm_write(pwm2,DUTY);
		mraa_pwm_write(pwm3,DUTY);
		mraa_pwm_write(pwm4,DUTY);
		for(j = PREAMBLE_DELAY; j > 0; j--);

		mraa_pwm_write(pwm1,0);
		mraa_pwm_write(pwm2,0);
		mraa_pwm_write(pwm3,0);
		mraa_pwm_write(pwm4,0);
		for(j = PREAMBLE_DELAY; j > 0; j--);
	}
}

void send_low_bit() {
	int i = 0;
	printf("0");

	mraa_pwm_write(pwm1,DUTY);
	mraa_pwm_write(pwm2,DUTY);
	mraa_pwm_write(pwm3,DUTY);
	mraa_pwm_write(pwm4,DUTY);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
	
	mraa_pwm_write(pwm1,0);
	mraa_pwm_write(pwm2,0);
	mraa_pwm_write(pwm3,0);
	mraa_pwm_write(pwm4,0);
	for(i = LONG_DELAY; i > 0; i--); // 20 ms
}

void send_high_bit() {
	int i = 0;
	printf("1");
	
	mraa_pwm_write(pwm1,DUTY);
	mraa_pwm_write(pwm2,DUTY);
	mraa_pwm_write(pwm3,DUTY);
	mraa_pwm_write(pwm4,DUTY);
	for(i = LONG_DELAY; i > 0; i--); //20 ms

	mraa_pwm_write(pwm1,0);
	mraa_pwm_write(pwm2,0);
	mraa_pwm_write(pwm3,0);
	mraa_pwm_write(pwm4,0);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
}

int check_edison_id() {
	int fd;
    unsigned char data;
    unsigned char buf[100];
    int i, n;
    int returnValue = -1;
    fd = open("/etc/IR_conf/edisonID.txt", O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("open edisonID.txt failed!\n");
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
    printf("Response from edisonID.txt: %s", buf);
	returnValue = buf[0];
 
    close(fd);
	return returnValue;
}

int main(int argc, char *argv[]) {
	
	printf("\n---------------------ir_transmit.c---------------------\n");
	//const struct sched_param priority={1};
	//sched_setscheduler(0,SCHED_FIFO,&priority);
	
	int transmit_counter = 0;
	int i = 0;
	int temp = -1;
	// GPIO Initialization - Edison has 4 PWM pins
	
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

	printf("\n---------------------Part 0: Apply Settings from Config Files---------------------\n");
	temp = check_preamble_length();
	if (temp != -1) {
		printf("\nGot Preamble Length from /etc/IR/preamble_length.txt\n");
		printf("ID I got was %d\n", temp);
		printf("But we are still going to use 5\n", temp);
		//preamble_length = temp;
	}
	temp = -1;
	
	// check file for edisonID
	temp = check_edison_id();
	if (temp != -1) {
		printf("Got Edison ID from /etc/IR/edisonID.txt\n");
		printf("ID I got was %d\n", temp);
		edisonID = temp;
	}
	temp = -1;
	
	printf("\n---------------------Part 0: Apply Settings from Passed in Arguments---------------------\n");
	// if we pass in an argument, use it for the preamble_length, takes
	// priority over what's in the file. please pass in an integer
	if (argc == 2) {
		preamble_length = atoi(argv[1]);
		printf("Argument(1) Passed In! Preamble Length set to: %d\n", preamble_length);
	}
	else {
		printf("No Argument(1), preamble Length set to default value: %d\n", preamble_length);
	}
	// if we pass in an argument, use it for the edisonID, takes
	// priority over what's in the file. please pass in an integer
	if (argc == 3) {
		preamble_length = atoi(argv[1]);
		printf("Argument(1) Passed In! Preamble Length set to: %d\n", preamble_length);
		
		edisonID = atoi(argv[2]);
		printf("Argument(2) Passed In! edisonID set to: %d\n", edisonID);		
	}
	else {
		printf("No Argument(2), edisonID set to default value: %d\n", edisonID);		
	}	
	// This will transmit IR data on all 4 pins at once
	// on a single Edison board

	
	// Continuously Transmit IR Signal when the program is running
	while(1) {
		printf("\n---------------------Transmission---------------------\n");
	
		// Updates the preamble_length variable based on info from server
		temp = check_preamble_length();
		if (temp != -1) {
			printf("ID from /etc/IR_conf/preamble_length.txt was %d\n", temp);
			preamble_length = temp;
		}
		
		// Preamble - Signals the Receiver Message Incoming
		printf("Sending preamble_length = %d, ", preamble_length);
		send_preamble_sequence(preamble_length);
		
		// Sending Edison Board ID # - 2 bits, MSB then LSB
		printf("EdisonID: %d - ", edisonID);
		switch (edisonID) {
			case 0:
				send_low_bit();		// Send lsb bit 0 = LOW
				send_low_bit();		// Send msb bit 1 = LOW
				break;
			case 1:
				send_low_bit();		// Send msb bit 1 = LOW
				send_high_bit();	// Send lsb bit 0 = HIGH
				break;
			case 2:
				send_high_bit();	// Send msb bit 1 = HIGH
				send_low_bit();		// Send lsb bit 0 = LOW
				break;
			case 3:
				send_high_bit();	// Send lsb bit 0 = HIGH
				send_high_bit();	// Send msb bit 1 = HIGH
				break;
			default:
				send_low_bit();		// Send lsb bit 0 = LOW
				send_low_bit();		// Send msb bit 1 = LOW				
		}
		
		// Sending Edison IR Emitter ID # - 2 bits, MSB then LSB
		
		// pwm1,DUTY = 00 = short-long/short-long = 5-20/5-20
		// pwm2,DUTY = 01 = short-long/long-short = 5-20/20-5
		// pwm3,DUTY = 10 = long-short/short-long = 20-5/5-20
		// pwm4,DUTY = 11 = long-short/long-short = 20-5/20-5
		
		printf(", EmitterIDs 0-3\n");
		// First Bit
		mraa_pwm_write(pwm1,DUTY);
		mraa_pwm_write(pwm2,DUTY);
		mraa_pwm_write(pwm3,DUTY);
		mraa_pwm_write(pwm4,DUTY);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		mraa_pwm_write(pwm1,0);
		mraa_pwm_write(pwm2,0);
		//mraa_pwm_write(pwm3,DUTY, DUTY);
		//mraa_pwm_write(pwm4,DUTY, DUTY);
		for(i = MID_DELAY; i > 0; i--); // 15 ms

		//mraa_pwm_write(pwm1, 0);
		//mraa_pwm_write(pwm2, 0);
		mraa_pwm_write(pwm3,0);
		mraa_pwm_write(pwm4,0);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		// Second Bit	
		mraa_pwm_write(pwm1,DUTY);
		mraa_pwm_write(pwm2,DUTY);
		mraa_pwm_write(pwm3,DUTY);
		mraa_pwm_write(pwm4,DUTY);
		for(i = SHORT_DELAY; i > 0; i--); // 20 ms

		mraa_pwm_write(pwm1,0);
		//mraa_pwm_write(pwm2,DUTY, DUTY);	
		mraa_pwm_write(pwm3,0);
		//mraa_pwm_write(pwm4,DUTY, DUTY);	
		for(i = MID_DELAY; i > 0; i--); // 15 ms

		//mraa_pwm_write(pwm1, 0);
		mraa_pwm_write(pwm2,0);
		//mraa_pwm_write(pwm3, 0);
		mraa_pwm_write(pwm4,0);
		for(i = SHORT_DELAY; i > 0; i--); // 5 ms
		
		/*
		send_message_bit(edisonID, 0, DUTY, pwm1,DUTY);
		send_message_bit(edisonID, 0, DUTY, pwm1,DUTY);
		send_message_bit(edisonID, 0, DUTY, pwm1,DUTY);
		send_message_bit(edisonID, 0, DUTY, pwm1,DUTY);
		
		
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 0, DUTY, pwm1,DUTY);
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 1, DUTY, pwm2,DUTY);			
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0; transmit_counter--) {
			send_message_bit(edisonID, 2, DUTY, pwm3,DUTY);
		}
		for (transmit_counter = TRANSMIT_DURATION; transmit_counter >= 0 ;transmit_counter--) {
			send_message_bit(edisonID, 3, DUTY, pwm4,DUTY);
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
