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

void led_all_on_off(int _s) {
	// LED_ALL_ON_OFF
	for (int i=0; i<3; i++) {
		
		PORTC = 0x00;
		_delay_ms(_s);
		PORTC = 0xFF;
		_delay_ms(_s);
	}
}
void led_alternating(int _s) {
	// LED_ALTERNATING
	for(int i=0;i<3;i++){
		
		PORTC = 0xAA;
		_delay_ms(_s);
		PORTC = ~0xAA;
		_delay_ms(_s);
	}
}
void led_shift(int _s) {
	
	// LED_SHIFT
	for (int j=0; j<3; j++) {
		
		for(int i=0; i<8; i++) {
			
			PORTC = ~(0x01 << i);
			if (i != 7) {
				_delay_ms(_s);
			}
		}
		for(int i=0; i<8; i++) {
			
			PORTC = ~(0x80 >> i);
			if (i != 7) {
				_delay_ms(_s);
			}
		}
	}
}

ISR(INT2_vect) {	// SW1
	cli(); // clear global interrupt
	EIFR |= 0x04;
	sei(); // set enable global interrupt
	
	// Do something when INT2 is invoked
	led_shift(200);
	_delay_ms(200);
	reti();
}

ISR(INT5_vect) {	// SW2
	cli();
	EIFR |= 0x20;
	sei();
	
	led_all_on_off(200);
	_delay_ms(200);
	reti();
}

ISR(PCINT2_vect) {	// SW3
	cli();
	unsigned char in = PINK & 0x04;
	sei();
	if((in & 0x04)) led_alternating(200);

	// led_alternating(200);

	PCIFR |= 0x04;
	_delay_ms(200);
	reti();
}

int main(void)
{
	cli();
	
	// INTn 이용
	EICRA = 0x30; // INT2 rising edge
	EICRB = 0x0C; // INT5 rising edge
	EIMSK = 0x24; // INT2, INT5 MASK
	EIFR = 0x24; // 처음에 flag 넣어준다.
	
	// PCINTn 이용
	PCICR = 0x04;
	PCIFR = 0x04;
	PCMSK2 = 0x04;
	
	
	/************************************************************************/
	
	
	PORTC = 0xFF;
	DDRC = 0xFF;
	
	
	/************************************************************************/

	
	// 인터럽트를 받기 위한 D, E 입력 설정
	DDRD &= ~(0x04);
	DDRE &= ~(0x20);
	
	// 인터럽트를 받기 위한 K 입력 설정
	DDRK &= ~(0x04);
	
	
	/************************************************************************/

	
	sei(); // enable global interrupt
	
	while(1) {

	}
	
}

