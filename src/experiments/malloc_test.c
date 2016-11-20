#include <stdio.h>
#include <stdlib.h>
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


// THIS IS OK
#define GETMEM 500000000
// THIS IS NOT OK
#define GETMEM 5000000000


int main(int argc, char *argv[]) {
	int *ptr_one;
	ptr_one = (int *)malloc(GETMEM*sizeof(int));

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}
	else {
		printf("SUCCESS: Obtained memory\n");
	}
	//*ptr_one = 25;
	//printf("%d located at %p\n", *ptr_one, ptr_one);

	free(ptr_one);
	
	return 0;
}//end main
