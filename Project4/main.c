/*
 * Project2.c
 *
 * Created: 4/21/2019 7:49:20 PM
 * Author : lydam
 */ 

#include "avr.h"
#include "lcd.h"
#include <stdio.h>

//globals
int keypad_pressed[16];

// determines if a key on the keypad is pressed
int is_pressed(int r, int c){
	// set all pins of c to N/C - weak 0.
	DDRC = 0;
	PORTC = 0;

	// set PORTC's # r GPIO to Weak 1
	SET_BIT(PORTC, r);
	// set PORTC's # c GPIO to strong 0
	SET_BIT(DDRC, c); // set direction to output
	CLR_BIT(PORTC, c); // strong 0
	if( GET_BIT(PINC, r) != 0){//check if r is not 0)
		// not pressed
		return 0;
	}
	else{
		// is pressed
		return 1;
	}
}

int get_key(){
	int r, c;
	for(r = 0; r < 4; r++){
		for(c = 0; c < 4; c++){
			if(is_pressed(r, c+4)){
				return (r*4 + c) + 1; // pin number
			}
		}
	}
	return 0;
}

/*
	Description: Check if the given input year, 
	causes the given days of the month to be exceeded
*/


int setup()
{
	lcd_init();
	lcd_clr();
	return 1;
}

/*
	[INPUT] a pointer to a tm struct
	DISPLAY: MM/DD/YYYY on the top row,
	 bottom row: HH:MM:SS
*/
/*
void display_time(struct tm *myTime){
	//local variables
	char top_row[11];
	char bottom_row[9];
	
	// TOP ROW
	sprintf(top_row, "%02d/%02d/%04d", myTime->month, myTime->day, myTime->year);
	lcd_pos(0, 2);
	lcd_puts2(top_row);
	
	// BOTTOM ROW
	sprintf(bottom_row, "%02d:%02d:%02d", myTime->hour, myTime->minute, myTime->second);
	lcd_pos(1, 2);
	lcd_puts2(bottom_row);
	
}
*/

/*
	Conversions:
	1 year = 12 months, 1 month = ?days, 1 day = 24 hours; 
	1 hour = 60 minutes; 1 minute = 60 seconds; 1 second = 1000ms
*/

void blink_led(int k){
	for(int i = 0; i < k; i++){
		SET_BIT(PORTA, 0);
		avr_wait(200);
		CLR_BIT(PORTA, 0);
		avr_wait(2000);
	}
}

/*
	Description: Blinks the current position
	on the lcd.
*/
void toggle_blink(struct tm* myTm, int r, int c){
	// get the value of the integer at r and c and store it to a variable x
	//int val = get_lcd_value(myTm, r, c);
	// set the value of the position at r and c to ' ' (space)
	lcd_pos(r, c);
	lcd_put(' ');
	avr_wait(60);
	//display_time(myTm);
	avr_wait(60);
}

void test_keypad(){
	int k;
	k = get_key();
	avr_wait(10);
	//blink LED k times
	if(k != 0){
		blink_led(k);
	}
	//long time led on (1sec)
	avr_wait(300);
	//led off
}

//====== PROJECT 4 ========================
typedef struct Samp{
	float instantaneous_voltage;
	float max_voltage;
	float min_voltage;
	float average_voltage;
	} Sample;

void set_sample(Sample *mysample,
	int intantaneous, int max, int min, int avg
	){
	mysample->instantaneous_voltage = intantaneous;
	mysample->max_voltage = max;
	mysample->min_voltage = min;
	mysample->average_voltage = avg;
}

float get_voltage(unsigned short x){
	return (x*5.0)/1023;
}

void ADC_init(){
	CLR_BIT(ADMUX, ADLAR);
	
	CLR_BIT(ADMUX, REFS1); //REFS1 = 0
	SET_BIT(ADMUX, REFS0); //REFS0 = 1
}

// should be a value from 0 - 1023
unsigned short sample(){
	SET_BIT(ADCSRA, ADEN);
	SET_BIT(ADCSRA, ADSC);
	while (GET_BIT(ADCSRA, ADSC) == 1) {}; // do nothing, conversion in progress
		
	//how to get result of conversion???
	//it is a register from ADC 
	unsigned short value = ADC;
	//CLR_BIT(ADCSRA, ADEN);
	return value; //ADC type float
	//return 0;
}

// displays instantaneous voltage, max voltage, min voltage, and average voltage
// 2 rows, 16 columns
// 4 columns for each number
void print_sample(Sample *sample){
	lcd_clr();
	//local variables
	char top_row[16];
	char bottom_row[16];
	int i1, i2, i3, i4, d1, d2, d3, d4 = 0;
	i1 = (int)sample->instantaneous_voltage;
	d1 = ((float)(sample->instantaneous_voltage - i1)*100);
	i2 = (int)sample->max_voltage;
	d2 = ((float)(sample->max_voltage - i2)*100);
	i3 = (int)sample->min_voltage;
	d3 = ((float)(sample->min_voltage - i3)*100);
	i4 = (int)sample->average_voltage;
	d4 = ((float)(sample->average_voltage - i4)*100);
	
	// TOP ROW
	sprintf(top_row, "I:%1d.%02d   Mx:%1d.%02d", i1, d1, i2, d2);
	lcd_pos(0, 0);
	lcd_puts2(top_row);
		
	// BOTTOM ROW
	sprintf(bottom_row, "Mn:%1d.%02d  A:%1d.%02d", i3, d3, i4, d4);
	lcd_pos(1, 0);
	lcd_puts2(bottom_row);
}

// updates the max, min, and average values
void update_sample(float new_instantaneous_voltage, Sample *sample){
	sample->instantaneous_voltage = new_instantaneous_voltage;
	sample->average_voltage = (new_instantaneous_voltage + sample->average_voltage)/2; //update average voltage
	if(new_instantaneous_voltage > sample->max_voltage){ // updating max voltage
		sample->max_voltage = new_instantaneous_voltage;
	}
	else if(new_instantaneous_voltage < sample->min_voltage || sample->min_voltage == 0){ // updating min
		sample->min_voltage = new_instantaneous_voltage;
	}

}

// on push of a button reset all voltage parameters
int reset_sample(Sample *mysample){
		mysample->instantaneous_voltage = 0;
		mysample->max_voltage = 0;
		mysample->min_voltage = 0;
		mysample->average_voltage = 0;
}

// ===== END PROJECT 4 ====================
int main(void)
{
	//local variables
	int k;
	int start_sample = 0;
	unsigned short samp;
	float s;
	Sample sp; // struct with fields, instantaneous, min, max, avg voltage.
	char buf[100];
	char buf2[100];
	// setting up
	set_sample(&sp, 1, 2, 3, 4);
	setup();
	ADC_init();
	
	// main logic
    while (1) 
    {
		lcd_pos(0,0);
		lcd_puts2("PRESS D TO START");
		// start sampling
		if(get_key() == 16){
			start_sample = 1;
		}
		while(start_sample){
			// reset sample
			if(get_key() == 12){ // User press C to reset.
				set_sample(&sp, 0, 0, 0, 0);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2("PRESS D TO START");
				start_sample = 0;
				break;
			}
			samp = sample(); // gets the instantaneous value // THIS IS THE ONLY THING NOT DONEEEE!!!!!!
			s = get_voltage(samp);
			update_sample(s, &sp); // updating sample
			print_sample(&sp); // printing the sample
			avr_wait(500);
		}
		
		/*
		unsigned short var = sample();
		sprintf(buf, "acd: %d", var);
		lcd_pos(0,0);
		lcd_puts2(buf);
		float s = get_voltage(var);
		int value = (int) s;
		int dec = ((int)(s*100))%100;
		sprintf(buf2, "%d.%d", value, dec);
		lcd_pos(1,0);
		lcd_puts2(buf2);
		
		avr_wait(500);
		*/
    }
}

