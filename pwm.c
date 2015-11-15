#include <stdio.h>
#include <mraa/pwm.h>
#include <time.h>

#define PREAMBLE_LENGTH 6
#define PREAMBLE_DELAY 750000 

int main(){
	
	int i,j;	
	mraa_pwm_context pwm;
	pwm = mraa_pwm_init(5);
	if (pwm == NULL) {
		return 1;
	}
		
	mraa_pwm_period_us(pwm, 26);
	mraa_pwm_enable(pwm, 1);

	float duty = .5f;
	mraa_pwm_write(pwm, duty);

	
	while(1){
		//////////////////////////////////////////
		////////// Key Signal
		//////////////////////////////////////////
		////////// Preamble - see README
		
			for(i = PREAMBLE_LENGTH; i > 0; i--){
				//high then low with for loop setting a wait time ~10 milliseconds
				mraa_pwm_write(pwm, duty);
				for(j=PREAMBLE_DELAY; j>0; j--);
				mraa_pwm_write(pwm, 0);
				for(j=PREAMBLE_DELAY; j>0; j--);
			}
			
		///////////////////////////////////////////
		////////// Sending Edison BOARD # Information in bits	
		///////////////////////////////////////////
		////////// Send msb bit 1 = LOW
		///////////////////////////////////////////	
				
				mraa_pwm_write(pwm, duty);
				for(j=350000;j>0;j--);		//busy wait for ~5  milliseconds
				mraa_pwm_write(pwm, 0);
				for(j=1400000;j>0;j--);		//busy wait for ~20 milliseconds	
	
		///////////////////////////////////////////
		////////// Send lsb bit 0 = HIGH
		///////////////////////////////////////////

				mraa_pwm_write(pwm, duty);
				for(j=350000;j>0;j--);		//busy wait for ~20 milliseconds
				mraa_pwm_write(pwm, 0);
				for(j=1400000;j>0;j--);		//busy wait for ~5 milliseconds
	
		///////////////////////////////////////////
		////////// Sending Edison IR Emitter # Information in bits	
		///////////////////////////////////////////
		////////// Send msb bit 2 = LOW
		///////////////////////////////////////////	

				mraa_pwm_write(pwm, duty);
				for(j=350000;j>0;j--);		//busy wait for ~5 milliseconds
				mraa_pwm_write(pwm, 0);
				for(j=1400000;j>0;j--);		//busy wait for ~20 milliseconds
		
		///////////////////////////////////////////
		////////// Send bit 1 = LOW
		///////////////////////////////////////////	

				mraa_pwm_write(pwm, duty);
				for(j=350000;j>0;j--);		//busy wait for ~5 milliseconds
				mraa_pwm_write(pwm, 0);
				for(j=1400000;j>0;j--);		//busy wait for ~20 milliseconds

		///////////////////////////////////////////
		////////// Send lsb bit 0 = HIGH
		///////////////////////////////////////////	

				mraa_pwm_write(pwm, duty);
				for(j=1400000;j>0;j--);		//busy wait for ~20 milliseconds
				mraa_pwm_write(pwm, 0);
				for(j=350000;j>0;j--);		//busy wait for ~5 milliseconds
		

		///////////////////////////////////////////
		////////// End of Information string
		///////////////////////////////////////////
	
	}// end while loop

	mraa_pwm_write (pwm, 0);
	mraa_pwm_enable(pwm,0);
	return 0;
}//end main
