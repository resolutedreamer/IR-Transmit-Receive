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

#define PREAMBLE_DURATION 6
#define TRANSMIT_DURATION 4

#define PREAMBLE_DELAY 750000 // 10-11 miliseconds
#define SHORT_DELAY 350000 // 5 miliseconds
#define LONG_DELAY 1400000 // 20 miliseconds


void send_preamble_sequence(int preamble_duration, mraa_pwm_context pwm, float duty) {
	int i = 0, j = 0;
	printf("\nSending Preamble. ");

	for(i = preamble_duration; i > 0; i--){
		
		//equal durations
		mraa_pwm_write(pwm, duty); // high
		for(j = PREAMBLE_DELAY; j > 0; j--);

		mraa_pwm_write(pwm, 0); // low
		for(j = PREAMBLE_DELAY; j > 0; j--);
	}
}

void send_low_bit(mraa_pwm_context pwm, float duty) {
	int i = 0;
	printf("0 ");
	
	mraa_pwm_write(pwm, duty);	// 5ms
	for(i = SHORT_DELAY; i > 0; i--);
	mraa_pwm_write(pwm, 0);		// 20ms
	for(i = LONG_DELAY; i > 0; i--);	
}

void send_high_bit(mraa_pwm_context pwm, float duty) {
	int i = 0;
	printf("1 ");
	
	mraa_pwm_write(pwm, duty);	//20 ms
	for(i = LONG_DELAY; i > 0; i--);
	mraa_pwm_write(pwm, 0);		// 5ms
	for(i = SHORT_DELAY; i > 0; i--);
}

void send_message_bit(int edisonID, int irEmitterID, float duty, mraa_pwm_context pin) {
	
	// Preamble - Signals the Receiver Message Incoming
	send_preamble_sequence(PREAMBLE_DURATION, pin, duty);
	
	// Sending Edison Board ID # - 2 bits, MSB then LSB
	switch (edisonID){
		case 0:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_low_bit(pin, duty);	// Send msb bit 1 = LOW
			break;
		case 1:
			send_high_bit(pin, duty);	// Send lsb bit 0 = HIGH
			send_low_bit(pin, duty);	// Send msb bit 1 = LOW
			break;
		case 2:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_high_bit(pin, duty);	// Send msb bit 1 = 
			break;
		case 3:
			send_high_bit(pin, duty);	// Send lsb bit 0 = HIGH
			send_high_bit(pin, duty);	// Send msb bit 1 = HIGH
			break;
		default:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_low_bit(pin, duty);	// Send msb bit 1 = LOW				
	}
	
	// Sending Edison IR Emitter ID # - 2 bits, MSB then LSB
	switch (irEmitterID) {
		case 0:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_low_bit(pin, duty);	// Send msb bit 1 = 
			break;
		case 1:
			send_high_bit(pin, duty);	// Send lsb bit 0 = HIGH
			send_low_bit(pin, duty);	// Send msb bit 1 = LOW
			break;
		case 2:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_high_bit(pin, duty);	// Send msb bit 1 = HIGH
			break;
		case 3:
			send_high_bit(pin, duty);	// Send lsb bit 0 = HIGH
			send_high_bit(pin, duty);	// Send msb bit 1 = HIGH
			break;
		default:
			send_low_bit(pin, duty);	// Send lsb bit 0 = LOW
			send_low_bit(pin, duty);	// Send msb bit 1 = LOW				
	}
}

int main(int argc, char *argv[]) {
	
	int transmit_counter = 0;
	// GPIO Initialization - Edison has 4 PWM pins
	float duty = .5f;
	
	mraa_pwm_context pwm1;
	pwm1 = mraa_pwm_init(5);
	mraa_pwm_period_us(pwm1, 26);
	mraa_pwm_enable(pwm1, 1);
	
	mraa_pwm_context pwm2;
	pwm2 = mraa_pwm_init(3);
	mraa_pwm_period_us(pwm2, 26);
	mraa_pwm_enable(pwm2, 1);
	
	mraa_pwm_context pwm3;
	pwm3 = mraa_pwm_init(6);
	mraa_pwm_period_us(pwm3, 26);
	mraa_pwm_enable(pwm3, 1);
	
	mraa_pwm_context pwm4;
	pwm4 = mraa_pwm_init(9);
	mraa_pwm_period_us(pwm4, 26);
	mraa_pwm_enable(pwm4, 1);
	
	// Continuously Transmit IR Signal when the program is running
	while(1) {
		// by default, the edison has ID 0
		int edisonID = 0;
		// if we pass in an argument, use it for the edisonID
		// please pass in an integer
		if (argc == 2) {
			edisonID = atoi(argv[1]);
		}
		// This will transmit IR data round-robin
		// on a single Edison board
		
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
