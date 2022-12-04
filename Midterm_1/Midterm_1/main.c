// 인터럽트 왜 안될까…

/*
 * 1025.c
 *
 * Created: 2022-10-25 오전 9:02:47
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__	// _delay_ms 함수 parameter 해결법
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>
#include <avr/interrupt.h> // 인터럽트 쓰려면 꼭 include 해야 할 헤더 파일

volatile unsigned char reverse = -1;

volatile unsigned int led1 = 0b00000001;
volatile unsigned int led2 = 0b10000000;

volatile unsigned int t = 125;

volatile unsigned int n = 0x00;

volatile unsigned char flag = 0;

volatile unsigned char SW1_flag = 0;

volatile unsigned char SW2_flag = 0;

volatile unsigned char SW3_flag = 0;


void led_shift(void) {
	
	if(reverse == 1) {
		PORTD = (0x01 << n);
		
		PORTK = (0x80 >> n);
	}
	
	else {
		PORTD = ~(0x01 << n);
	
		PORTK = ~(0x80 >> n);
	}
		
		if(flag==1) {
			n--;
			if(n==0) flag=0;
		}
		
		else {
			n++;
		}
		
		if(n==4) flag=1;
		
		_delay_ms(t);
	
			
}

ISR(INT4_vect) {
	_delay_ms(200);
	
	cli();
	
	EIFR |= 0x10;
	
	sei();
	
	unsigned int in = (0b00010000 & PINE);
	
		if(SW3_flag==1) {
			if(t<=125) t=125;
			else t=t-125;
			_delay_ms(200);
		}
		
			if(!in) {
				while((0b00010000 & PINE)) {
					SW2_flag=1;
				}
			}
}

ISR(INT5_vect) {
	_delay_ms(200);
	
	cli();
	
	EIFR |= 0x20;
	
	sei();
	
	unsigned int in = (0b00100000 & PINE);
	
	if(!in) {
		while((0b00100000 & PINE)) {
			SW3_flag=1;
		}
	}
	
		if(SW2_flag==1) {
			if(t>=1125) t=1125;
			else t=t+125;
			_delay_ms(200);
		}

	
}



int main(void)
{
	cli();
	

	EICRB = 0x0F; // INT4, INT5 rising edge
	EIMSK = 0x30; // INT4, INT5 MASK
	EIFR = 0x30; // 처음에 flag 넣어준다.
	
	
	/************************************************************************/
	
	DDRD |= 0b00001111;
	DDRK |= 0b11111000;
	
	DDRC = 0xF8;
	
	DDRE &= ~(0x30);
	
	
	PORTD = 0xFF;
	PORTK = 0xFF;
	
	
	/************************************************************************/

	unsigned int in1;
	
	unsigned int in2;
	
	
	/************************************************************************/
	
	
	sei(); // enable global interrupt
	
	while(1) {
		
		SW1_flag=0;
		
		SW2_flag=0;
		
		in1 = (PINC & 0x01);
			
		if (!in1) {
			// _delay_ms(200);
			while(!(PINC & 0x01)){
				led_shift();
				_delay_ms(t);
			}
				
			reverse = -reverse;
		}
			
		led_shift();
		
		_delay_ms(t);
		
		led1 << 1;
		
	}
	
}


