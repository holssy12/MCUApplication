/*
 * 0927.c
 *
 * Created: 2022-09-27 오전 9:57:52
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>


/*
int main(void)
{
    // Replace with your application code
	
	DDRC = 0xFF;
	
	while (1) 
    {
		PORTC = 0x00;
		_delay_ms(500);
		PORTC = 0xFF;
		_delay_ms(500);
    }
}
*/

void led_all_onoff(int _n, int _s) {

	
	for (int i = 0; i < _n; i++){
		PORTC = 0x00;	// 전체 led on
		_delay_ms(_s);
		PORTC = 0xFF;
		_delay_ms(_s);	// 전체 led off
	}
}

int main(void) {
	DDRC=0xFF;
	led_all_onoff(7, 300);
}
