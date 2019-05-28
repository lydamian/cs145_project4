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
	SET_BIT(DDRA, 0);
	CLR_BIT(PORTA, 0);
	lcd_init();
	lcd_clr();
	SET_BIT(DDRA, 0);
	
	return 1;
}

/*
	[INPUT] a pointer to a tm struct
	DISPLAY: MM/DD/YYYY on the top row,
	 bottom row: HH:MM:SS
*/
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
	display_time(myTm);
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

int main(void)
{
	//local variables
	int k;
	struct tm
	
	// setting up
	setup();
	
	// main logic
    while (1) 
    {	
    }
}

