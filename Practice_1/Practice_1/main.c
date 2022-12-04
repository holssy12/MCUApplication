/*
 * Practice_1.c
 *
 * Created: 2022-10-30 오후 11:49:19
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

void led_all(unsigned int _s) {
	for(int i=0; i<3; i++){
		PORTC = 0x00;
		_delay_ms(_s);
		PORTC = 0xFF;
		_delay_ms(_s);
	}
}

void led_alternating(unsigned int _s){
	for(int i=0; i<3; i++){
		PORTC = 0xAA;
		_delay_ms(_s);
		PORTC = ~0xAA;
		_delay_ms(_s);
	}
}

void led_shift(unsigned int _s){
	unsigned int val;
	int i;
	for(int j=0; j<3; j++){
		
		if(j==0) i=0;
		else i=1;
		
		for( ; i<15; i++){
			if(i<7) {
				val = 0x01;
				PORTC = ~(val << i);
			}
			else {
				val = 0x80;
				PORTC = ~(val >> (i-7));
			}
			_delay_ms(_s);
		}
	}
}

ISR(INT2_vect){
	cli();
	EIFR|=0x04;
	sei();
	led_shift(100);
	reti();
}

ISR(INT5_vect){
	cli();
	EIFR |= 0x20;
	sei();
	led_all(200);
	reti();
}

ISR(PCINT2_vect){
	cli();
	unsigned int in = PINK & (0x04);
	PCIFR |= 0x04;
	sei();
	if(!in) led_alternating(200);
	else return 0;
	reti();
}

int main(void)
{
	cli();
	
	EICRA = 0x20;
	EICRB = 0x08;
	EIMSK = 0x24;
	EIFR =  0x24;
	
	PCICR = 0x04;
	PCIFR = 0x04;
	PCMSK2 = 0x04;
	
	PORTC = 0xFF;
	DDRC = 0xFF;
	
	DDRD = ~(0x04);
	DDRE = ~(0x20);
	
	DDRK = ~(0x04);
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		
    }
}

