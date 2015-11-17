/////////////////////////////////////////////////////////////////////////
//
//	irRecvr_pr_2.c
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
#define SAMPLING_RATE 32000

#define UPPERBOUND_PREAMBLE 24
#define LOWERBOUND_PREAMBLE 16

#define UPPERBOUND_LOWVALUE 15
#define LOWERBOUND_LOWVALUE 5

#define UPPERBOUND_HIGHVALUE 45
#define LOWERBOUND_HIGHVALUE 35

int main(int argc, char *argv[]) {
	
	// Initialization
	volatile int irSig = 0 ;
	// raw_data stores 0 or 1 dep on what was received
	int raw_data[3000] ;  
	// values stores the number of 0's or the number of 1's 
	unsigned int values[1000];

	volatile unsigned int i = 0, j = 0, k = 0;
	volatile unsigned int num_values = 0;
	volatile unsigned int l = 0;
	volatile unsigned int received_bit_count = 0;
	int bit = -1;
	int all_received_bits[25] = {-1};
	
	unsigned int counting_high_or_low;
	unsigned int high_duration = 0;
	unsigned int low_duration = 0;
	unsigned int count = 0;
	
	unsigned int fsm_state = 0  ; 
	unsigned int preamble_found = 1 ; 
	mraa_gpio_context gpio;
	gpio = mraa_gpio_init(4);
	mraa_gpio_dir(gpio, MRAA_GPIO_IN);
	
	// Part 1: Raw Sampling
	// Detect the value received as either 1 or 0
	// Store in raw_data[]
	printf("Beginning Sample Collection\n");
    while(count < 3000 ) {
		irSig = mraa_gpio_read(gpio);
		raw_data[count] = irSig ; 
		printf("%d", raw_data[count]);
		for (j = 0 ; j < SAMPLING_RATE ; j++);
		count++;
	}//end while loop

	// Part 2: Consolidating the bits
	// Count the sequence of 1 or 0 received in a row
	// Store in values[]
	counting_high_or_low = raw_data[0];
	printf("The first bit we received was raw_data[0] = %u", raw_data[0]);
	printf("Therefore we are counting a stream of digits of: %u", counting_high_or_low);
	if (raw_data[0] == 0) {
		low_duration = 1;
	}
	else if (raw_data[0] == 1) {
		high_duration = 1;
	}
	count = 1;	
	while( count < 3000 )
	{
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
			  high_duration++ ; 
			}                 
	    }
		else {
			printf("raw_data[count] error");
		}
		count++;    
	}/*end of while loop */


	printf("\n");
	num_values = i ; 
	printf("num_values = %u\n", num_values);
	for (i = 0 ; i < num_values; i++ ) {
		printf("%u \n", values[i]);
	}
	
	// Part 3: Bit Decoding FSM
	// Search values[] for sequences of preamble and 4 bits
	// Store in all_received_bits[]
	i = 0;
	j = 0;
	k = 0;
	while (i < num_values)
	{
		printf("Iteration %u out of %u\n", i, num_values) ; 
		for (j = i; j < (i+12); j++)
		{
			printf("Inspecting j = %u with the if statement\n", values[j]);
			if (values[j] < LOWERBOUND_PREAMBLE || values[j] > UPPERBOUND_PREAMBLE)  
			{
				printf("Did not find preamble \n" );
				preamble_found = 0;
				break;
			}
		}
		if (preamble_found == 1)
		{
			printf("We found the preamble, now we need to skip 12 values.\n");
			for(l = 0; l < 12; l ++) {
				printf("Skipping i = %u, values[i] = %u\n", i, values[i]);
				i++;
			}
			printf("Now we are at the first of the 8 values for the 4 bits.\n");
			for(k = 1 ; k <= 4 ; k++)
			{
				printf("Constituents of the bit %u %u \n ", values[i],values[i+1]);
				
				bit = -1;
				
				if( values[i]>=LOWERBOUND_HIGHVALUE && values[i] <= UPPERBOUND_HIGHVALUE && values[i+1] >=LOWERBOUND_LOWVALUE && values[i+1] <= UPPERBOUND_LOWVALUE )
				{
					bit = 1;
				}
				
				if( values[i]>=LOWERBOUND_LOWVALUE && values[i] <= UPPERBOUND_LOWVALUE && values[i+1] >=LOWERBOUND_HIGHVALUE && values[i+1] <= UPPERBOUND_HIGHVALUE)
				{
					bit = 0;
				}
				
				i += 2;
				printf("bit = %d\n", bit);
				
				all_received_bits[received_bit_count] = bit;
				received_bit_count++;
			} 
			printf("\n");
		}
		else {
			i++;
			preamble_found = 1;				
		}
	}
	
	// Part 4: Bit Verifier
	// Inspect all_received_bits[]
	// Store in TEMP
	printf("Okay, let's look at the bits we received\n");
	i = 0;
	j = 0;

	int valid_sequence = 1;
	int location[6] = {0, 0, 0, 0, 0, 0};

	for(i = 0; i < 4*6; i += 4) {
		printf("\nReceiving EdisonID/LEDID Unit\n");
		for (j = 0; j < 4; j++) {
			printf("message[%u] = %d\n", j, all_received_bits[i+j]);
			if (all_received_bits[i+j] == -1)
			{
				valid_sequence = 0;
			}
		}
		if (valid_sequence == 1) {
			location[i/4] =  all_received_bits[i+3]* 1 + all_received_bits[i+2]* 2 + all_received_bits[i+1] * 4 + all_received_bits[i+0] * 8;
			// locations go from 1-16
			printf("\ni = %d, location[i] is : %d", i, location[i/4]);
			location[i/4] = location[i/4] + 1;
			printf("\nReceived a location: %d\n", location[i/4]);
		}
		else {
			printf("Received an invalid sequence\n");
			valid_sequence = 1;
		}
	}
	
	for (i = 0 ; i < 6; i++ ) {
		printf("%d \n", location[i]);
	}
	
	// Part 5: Valid Sequence Verifier
	// if we find a total of 3 matching
	// bit sequences (out of 5), return
	int match_counter = 0;
	for(i = 0; i < 6; i++) {
		for (j = 0; j < 4; j++) {
			if (location[i] == location[j]) {
				match_counter++;
			}
			if (match_counter == 4) {
				printf("\nReturning location[i] = %d\n", location[i]);
				return location[i];
			}
		}
		match_counter = 0;
	}
	
	/*
	// THIS PRINT WORKS DONT BREAK IT
	for(i = 0; i < 100; i += 1) {
		printf("all_received_bits[%u] = %d\n", i, all_received_bits[i]);
	}
	*/
	
	return 0;
}//end main
