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
#define PREAMBLE_DURATION 5

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
    volatile int irSig = 0 ;
    // values stores the number of 0's or the number of 1's
    unsigned int values[100];
    
    volatile unsigned int i = 0, j = 0, k = 0;
    volatile unsigned int num_values = 0;
    volatile unsigned int l = 0;
    volatile unsigned int received_bit_count = 0;
    int bit = -1;
    int all_received_bits[25];
    int preamble_relaxed_detection ;
    
    mraa_gpio_context gpio;
    gpio = mraa_gpio_init(4);
    mraa_gpio_dir(gpio, MRAA_GPIO_IN);
    
    
    // Part 1: Raw Data Sampling
    // Detect the value received as either 1 or 0
    // Store in raw_data[]
    printf("Part 1: Raw Data Sampling\n");
    printf("Beginning Sample Collection\n");
    // raw_data stores 0 or 1 dep on what was received
    int raw_data[TOTAL_SAMPLES] ;
    unsigned int samples_remaining = TOTAL_SAMPLES;
    i = 0;
    while(samples_remaining > 0 ) {
        irSig = mraa_gpio_read(gpio);
        raw_data[i] = irSig ;
        printf("%d", raw_data[i]);
        for (j = 0 ; j < SAMPLING_RATE ; j++);
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
    
    printf("\nThe first bit we received was raw_data[0] = %u\n", raw_data[0]);
    printf("Therefore we are counting a stream of digits of: %u", counting_high_or_low);
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
    
    // Part 3: Bit Decoding FSM
    // Search values[] for sequences of preamble and 4 bits
    // Store in all_received_bits[]
    i = 0;
    j = 0;
    k = 0;
    unsigned int fsm_state = 0 ;
    unsigned int preamble_found = 1 ;
    printf("\nPart 3: Bit Decoding FSM\n");
    printf("Begin Decoding\n");
    
    while (i < num_values)
    {
        printf("Iteration %u out of %u\n", i, num_values) ;
        preamble_relaxed_detection = 0 ;
        for (j = i; j < ( i + PREAMBLE_DURATION*2 ); j++)
        {
            printf("Inspecting j = %u with the if statement\n", values[j]);
            
            if (values[j] < LOWERBOUND_PREAMBLE || values[j] > UPPERBOUND_PREAMBLE)
            {
                if (values[j] > LOWERBOUND_PREAMBLE - 5 && values[j] < UPPERBOUND_PREAMBLE + 5 )
                {
                    if ( preamble_relaxed_detection < PREAMBLE_RELAXED_DETECTION_THRESHOLD )
                    {
                        preamble_relaxed_detection++ ;
                        continue ;
                    }
                }
                printf("Did not find preamble \n" );
                preamble_found = 0;
                break;
            }
        }
        if (preamble_found == 1)
        {
            printf("We found the preamble, now we need to skip PREAMBLE_DURATION * 2 values.\n");
            for(l = 0; l < PREAMBLE_DURATION * 2; l ++) {
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
    
    printf("FSM Complete\n\n\n\n");
    int messages = received_bit_count/4;
    printf("Identified %d bits\n", received_bit_count);
    printf("This corresponds to %d messages\n", messages);
    
    
    printf("\nNow we print the all_received_bits[] array.\n");
    num_values = i ;
    for (i = 0 ; i < received_bit_count; i++ ) {
        printf("all_received_bits[%u] = %d\n", i, all_received_bits[i]);
    }
    
    
    // Part 4: Bit Verifier
    // Inspect all_received_bits[]
    // Store in TEMP
    printf("\nPart 4: Bit Verifier\n");
    printf("Begin Bit Inspection\n");
    i = 0;
    j = 0;
    
    int valid_sequence = 1;
    int location[6] = {0, 0, 0, 0, 0, 0};
    int messages_to_process = 0;
    if (messages < 6) {
        messages_to_process = messages;
    }
    else {
        messages_to_process = 6;
    }
    // we process at most 6 messages, at minimum 0
    
    for(i = 0; i < messages_to_process * 4; i += 4) {
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
    
    printf("Done Bit Inspection\n");
    if (messages_to_process > 0) {
        printf("Messages Correspond to Locations:\n");
        for (i = 0 ; i < messages_to_process; i++ ) {
            printf("%d \n", location[i]);
        }
        
        // Part 5: Valid Sequence Verifier
        // if we find a total of 3 matching
        // bit sequences (out of 5), return
        printf("\nPart 5: Valid Sequence Verifier\n");
        printf("Begin Counting Matching Bits\n");
        int match_counter = 0;
        for(i = 0; i < messages_to_process; i++) {
            for (j = 0; j < 4; j++) {
                if (location[i] == location[j]) {
                    match_counter++;
                }
                if (match_counter >= LOCATION_DETECTION_THRESHOLD && location[i] != 0 ) {
                    printf("Done Counting Matching Bits\n");
                    printf("\nReturning location[i] = %d\n", location[i]);
                    return location[i];
                }
            }
            match_counter = 0;
        }
    }
    printf("\nReturning Zero Location");
    printf("Not Enough Bits Matching, Return 0\n");	
    return 0;
}//end main