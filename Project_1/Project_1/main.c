/*
 * Project_1.c
 *
 * Created: 2022-10-12 오후 5:13:38
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__	// _delay_ms 함수 parameter 해결법
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

void led_all_on_off(int _s) {	
		
	// LED_ALL_ON_OFF
	for (int i=0; i<3; i++) {
		PORTC = 0x00;
		PORTA = 0x00;
		_delay_ms(_s);
		PORTC = 0x0F;
		PORTA = 0xF0;
		_delay_ms(_s);
	}
}

void led_alternating(int _s) {
	// LED_ALTERNATING
	for (int i = 0; i < 3; i++) {
		PORTC = 0x0A;
		PORTA = 0xA0;
		_delay_ms(_s);
		PORTA = 0x50;
		PORTC = 0x05;
		_delay_ms(_s);
	}
	for (int i = 0; i < 3; i++) {
		PORTC = 0x00;
		PORTA = 0xF0;
		_delay_ms(_s);
		PORTC = 0x0F;
		PORTA = 0x00;
		_delay_ms(_s);
	}
	for (int i = 0; i < 3; i++) {
		PORTC = 0x03;
		PORTA = 0xC0;
		_delay_ms(_s);
		PORTC = 0x0C;
		PORTA = 0x30;
		_delay_ms(_s);
	}
}

void led_shift(int _s) {
	
	// LED_SHIFT
	for (int j=0; j<5; j++) {
		for(int i=0; i<8; i++) {
			PORTC = PORTA = ~(0x01 << i);
			if (i != 7) {
				_delay_ms(_s);
			}
		}
		for(int i=0; i<8; i++) {
			PORTC = PORTA = ~(0x80 >> i);
			if (i != 7) {
				_delay_ms(_s);
			}
		}
	}
}

void project_1(int _ms) {
	
	led_all_on_off(_ms);
	led_alternating(_ms);
	led_shift(_ms);
}


int main(void)
{
	DDRC=0x0F;
	DDRA=0xF0;
	
    /* Replace with your application code */
    while(1) {
		project_1(200);
	}
}

