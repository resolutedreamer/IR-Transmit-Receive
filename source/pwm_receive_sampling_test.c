/////////////////////////////////////////////////////////////////////////
//
//	pwm_receive.c
//
//	Original Author:
//	Pranjal Rastogi
//
// 	Contributing Authors:
// 	Anthony Nguyen
// 	Raymond Andrade
//
// 	Parameters:
// 	argc and argv
// 
//	// currently no inputs
//	// 
//
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <mraa/gpio.h>

// this is the rate for the "for loop" which waits
#define SCALING_FACTOR 5
#define SAMPLING_RATE 30000/SCALING_FACTOR
#define MAX_SAMPLING_RATE 30000

#define PREAMBLE_DURATION 5
int preamble_duration = 5;

#define TOTAL_SAMPLES 3000

#define UPPERBOUND_PREAMBLE 24
#define LOWERBOUND_PREAMBLE 16

#define UPPERBOUND_LOWVALUE 15
#define LOWERBOUND_LOWVALUE 5

#define UPPERBOUND_HIGHVALUE 45
#define LOWERBOUND_HIGHVALUE 35

#define PREAMBLE_RELAXED_DETECTION_THRESHOLD 2 
// this  indicates how many values from among 10 that 
//are examined can be outside the range of 15 to 25 , but still we can go ahead with preamble detection
// For example if preamble_relaxed_detection = 1 , one value out of 10 can be 13 and still we detect preamble
#define LOCATION_DETECTION_THRESHOLD 1
// this indicates number of matching locations for us to conclude that we detected the location correcly
// for example if LOCATION_DETECTION_THRESHOLD is set to 2 , then if we got 2 out of 5 locations as 13 , we
// can conclude that location was indeed 13 
// getting 3 out of 5 locaitons as matching is rather a stringent criterion 
// and will result in sending a zero location to the server 



int main(int argc, char *argv[]) {
	
	// Initialization	
	volatile unsigned int i = 0, j = 0, k = 0;
	volatile unsigned int num_values = 0;
	volatile unsigned int l = 0;
	volatile unsigned int received_bit_count = 0;
	int bit = -1;
	int all_received_bits[25];
	int preamble_relaxed_detection;  
	
	mraa_gpio_context gpio;
	gpio = mraa_gpio_init(8); // Arduino Pin 8 is Edison Pin 49
	mraa_gpio_dir(gpio, MRAA_GPIO_IN);
	
	
	// Part 0: Determine Preamble Duration
	// Detect the length of the preamble we expect from this message
	// Defaults to 5, get passed in as the 1st argument to pwm_receive
	printf("\nPart 0: Determine Preamble Duration\n");
	if (argc == 2) {
		preamble_duration = atoi(argv[1]);
	}
	printf("Preamble Length is %d", preamble_duration);
	
	// Part 1: Raw Data Sampling
	// Detect the value received as either 1 or 0
	// Store in raw_data[]
	printf("\nPart 1: Raw Data Sampling\n");
	printf("Beginning Sample Collection\n");
    // raw_data stores 0 or 1 dep on what was received
	int raw_data[TOTAL_SAMPLES] ;  
	unsigned int samples_remaining = TOTAL_SAMPLES;
	volatile int irSig = 0 ;
	i = 0;
	while(samples_remaining > 0 ) {
		irSig = mraa_gpio_read(gpio);
		raw_data[i] = irSig ; 
		printf("%d", raw_data[i]);
		for (j = 0 ; j < MAX_SAMPLING_RATE ; j++);
		//for (j = 0 ; j < SAMPLING_RATE ; j++);
		i++;
		samples_remaining--;
	}//end while loop
	i = 0;


	printf("\nSample Collection Complete\n");
    
	// Part 2: Consolidating the bits
	// Count the sequence of 1 or 0 received in a row
	// Store in values[]

	printf("\nPart 2: Consolidating the bits");
	unsigned int counting_high_or_low = raw_data[0];
	unsigned int high_duration = 0;
	unsigned int low_duration = 0;
	// values stores the number of 0's or the number of 1's 
	unsigned int values[100];

	printf("\nraw_data[0] = %u, counting %u's\n", raw_data[0], counting_high_or_low);
	if (raw_data[0] == 0) {
		low_duration = 1;
	}
	else if (raw_data[0] == 1) {
		high_duration = 1;
	}
	int count = 1;
	while( count < TOTAL_SAMPLES )
	{
		//printf("count = %d\n", count);
		//printf("raw_data[count] = %d\n", raw_data[count]);
			
		if ( raw_data[count] == 0 ) 
		{ 
			if (counting_high_or_low == 1)
		    {
				counting_high_or_low = 0; 
				low_duration = 1;
				values[i] = high_duration;
				i++;
			}
			else
			{
				low_duration++;
				//continue;
			}              
		}
		else if (raw_data[count] == 1)
		{
			if ( counting_high_or_low == 0 )
			{
				counting_high_or_low = 1; 
				high_duration = 1;
				values[i] = low_duration;
				i++;
			}
			else
			{
				high_duration++;
				//continue;
			}                 
	    }
		else {
			printf("\nraw_data[count] error\n");
			printf("count = %d\n", count);
			printf("raw_data[count] = %d\n", raw_data[count]);
			
		}
		count++;    
	}/*end of while loop */

	printf("\nNow we print the values[] array, which stores the result of what we counted.\n");
	num_values = i ; 
	printf("num_values = %u\n", num_values);
	for (i = 0 ; i < num_values; i++ ) {
		printf("%u \n", values[i]);
	}
	
	return 0;
}//end main
