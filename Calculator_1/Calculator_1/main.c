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

volatile unsigned int n1 = 0;
volatile unsigned int n2 = 0;

void led_alternating(unsigned int _s){
	for(int i=0; i<3; i++){
		PORTC = 0xAA;
		_delay_ms(_s);
		PORTC = ~0xAA;
		_delay_ms(_s);
	}
}

void num_1(void) {
	
	if(n1 == 0x0F) {
		n1=0; 
		PORTC |= 0x0F;
		return 0;
	}
	
	n1++;
	
	unsigned int t = 0xFF & n1;
	
	PORTC |= 0x0F;
	PORTC &= ~t;
}

void num_2(void) {
	
	if(n2 == 0x0F){ 
		n2=0;
		PORTC |= 0xF0;
		return;
	}
	
	n2++;
	
	unsigned int t = 0xFF & n2;
	
	PORTC |= 0xF0;
	PORTC &= ~(t<<4);
}

void sum(void) {
	
	unsigned int t = 0xFF & (n1 + n2);
	
	PORTC |= 0xFF;
	PORTC &= ~t;
}


ISR(INT2_vect){
	_delay_ms(200);
	cli();
	EIFR|=0x04;
	sei();
	num_1();
	//reti();
}

ISR(INT5_vect){
	_delay_ms(200);
	cli();
	EIFR |= 0x20;
	sei();
	num_2();
	//reti();
}

ISR(PCINT2_vect){
	_delay_ms(200);
	cli();
	unsigned int in = PINK & (0x04);
	PCIFR |= 0x04;
	sei();
	if(!in) {
		sum();
	}
	else {
		n1=0;
		n2=0;
		PORTC = 0xFF;
		reti();
	}
	// reti();
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

