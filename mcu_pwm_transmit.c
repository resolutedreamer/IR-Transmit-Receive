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

#include "mcu_api.h"
#include "mcu_errno.h"

// Standard PREAMBLE
#define PREAMBLE_DURATION 4

#define PREAMBLE_DUTY 50000
#define PREAMBLE_PERIOD 100000
#define PREAMBLE_SLEEP 100

#define MESSAGE_DUTY_LOW 20000
#define MESSAGE_DUTY_HIGH 80000
#define MESSAGE_PERIOD 100000
#define MESSAGE_SLEEP MESSAGE_PERIOD/1000


#define BUFFER_LENGTH 9
char host_message[BUFFER_LENGTH] = "5";
char preamble_length = 5;

#define pwm1 0
#define pwm2 1
#define pwm3 2
#define pwm4 3

void send_preamble_sequence(int preamble_duration) {
	//debug_print(DBG_INFO, "Preamble\n");
	pwm_configure(pwm1, PREAMBLE_DUTY, PREAMBLE_PERIOD);
	pwm_configure(pwm2, PREAMBLE_DUTY, PREAMBLE_PERIOD);
	pwm_configure(pwm3, PREAMBLE_DUTY, PREAMBLE_PERIOD);
	pwm_configure(pwm4, PREAMBLE_DUTY, PREAMBLE_PERIOD);

	pwm_enable(pwm1);
	pwm_enable(pwm2);
	pwm_enable(pwm3);
	pwm_enable(pwm4);

	mcu_delay(preamble_duration*PREAMBLE_SLEEP);
}

void send_low_bit() {
	//debug_print(DBG_INFO, "0 \n");
	pwm_configure(pwm1, MESSAGE_DUTY_LOW, MESSAGE_PERIOD); // 0
	pwm_configure(pwm2, MESSAGE_DUTY_LOW, MESSAGE_PERIOD);
	pwm_configure(pwm3, MESSAGE_DUTY_LOW, MESSAGE_PERIOD);
	pwm_configure(pwm4, MESSAGE_DUTY_LOW, MESSAGE_PERIOD);

	pwm_enable(pwm1);
	pwm_enable(pwm2);
	pwm_enable(pwm3);
	pwm_enable(pwm4);

	mcu_delay(MESSAGE_SLEEP);
}

void send_high_bit() {
	//debug_print(DBG_INFO, "1 \n");
	pwm_configure(pwm1, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD); // 1
	pwm_configure(pwm2, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD);
	pwm_configure(pwm3, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD);
	pwm_configure(pwm4, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD);

	pwm_enable(pwm1);
	pwm_enable(pwm2);
	pwm_enable(pwm3);
	pwm_enable(pwm4);

	mcu_delay(MESSAGE_SLEEP);
}


void mcu_main()
{
	// by default, the edison has ID 0
	int edisonID = 0;
	// if we pass in an argument, use it for the edisonID
	// please pass in an integer
	//if (argc == 2) {
		//edisonID = atoi(argv[1]);
	//}

	int temp;
	while (1)
	{

		//int host_receive(unsigned char *buf, int length)
		temp = host_receive((unsigned char *)host_message, BUFFER_LENGTH);
		if (temp > 0)
		{
			debug_print(DBG_INFO, "Received a Message!\n");
            host_send((unsigned char*)"hello mcu\n", 10);
            preamble_length = host_message[0];
		}

		// Preamble - Signals the Receiver Message Incoming
		send_preamble_sequence(preamble_length);

		// Sending Edison Board ID # - 2 bits, MSB then LSB
		switch (edisonID) {
			case 0:
				send_low_bit();	// Send lsb bit 0 = LOW
				send_low_bit();	// Send msb bit 1 = LOW
				break;
			case 1:
				send_high_bit();	// Send lsb bit 0 = HIGH
				send_low_bit();	// Send msb bit 1 = LOW
				break;
			case 2:
				send_low_bit();	// Send lsb bit 0 = LOW
				send_high_bit();	// Send msb bit 1 =
				break;
			case 3:
				send_high_bit();	// Send lsb bit 0 = HIGH
				send_high_bit();	// Send msb bit 1 = HIGH
				break;
			default:
				send_low_bit();	// Send lsb bit 0 = LOW
				send_low_bit();	// Send msb bit 1 = LOW
		}

		// Sending Edison IR Emitter ID # - 2 bits, MSB then LSB

		// pwm1 = 00 = short-long/short-long = 5-20/5-20
		// pwm2 = 01 = short-long/long-short = 5-20/20-5
		// pwm3 = 10 = long-short/short-long = 20-5/5-20
		// pwm4 = 11 = long-short/long-short = 20-5/20-5

		// First Bit
		pwm_configure(pwm1, MESSAGE_DUTY_LOW, MESSAGE_PERIOD); // 0
		pwm_configure(pwm2, MESSAGE_DUTY_LOW, MESSAGE_PERIOD); // 0
		pwm_configure(pwm3, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD); // 1
		pwm_configure(pwm4, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD); // 1

		pwm_enable(pwm1);
		pwm_enable(pwm2);
		pwm_enable(pwm3);
		pwm_enable(pwm4);

		mcu_delay(MESSAGE_SLEEP);

		// Second Bit
		pwm_configure(pwm1, MESSAGE_DUTY_LOW, MESSAGE_PERIOD); // 0
		pwm_configure(pwm2, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD); // 1
		pwm_configure(pwm3, MESSAGE_DUTY_LOW, MESSAGE_PERIOD); // 0
		pwm_configure(pwm4, MESSAGE_DUTY_HIGH, MESSAGE_PERIOD); // 1

		pwm_enable(pwm1);
		pwm_enable(pwm2);
		pwm_enable(pwm3);
		pwm_enable(pwm4);

		mcu_delay(MESSAGE_SLEEP);
	}


}
