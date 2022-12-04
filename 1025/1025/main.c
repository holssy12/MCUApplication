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

int int_status;

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
	for (int i = 0; i < 3; i++) {
		PORTC = 0xAA;
		_delay_ms(_s);
		PORTC = 0x55;
		_delay_ms(_s);
	}
	for (int i = 0; i < 3; i++) {
		PORTC = 0xF0;
		_delay_ms(_s);
		PORTC = 0x0F;
		_delay_ms(_s);
	}
	for (int i = 0; i < 3; i++) {
		PORTC = 0xC3;
		_delay_ms(_s);
		PORTC = 0x3C;
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

ISR(INT2_vect) {
	cli(); // clear global interrupt
	EIFR |= 0x04;
	sei(); // set enable global interrupt
	
	// Do something when INT2 is invoked
	led_all_on_off(200);
}

ISR(INT4_vect) {
	cli();
	EIFR |= 0x10;
	sei();
	
	led_shift(200);	
}

ISR(INT5_vect) {
	cli();
	EIFR |= 0x20;
	sei();
	
	led_alternating(200);
}

ISR(PCINT2_vect) {
	_delay_ms(100);
	cli();
	unsigned char in = PINK & 0x07;
	sei();
	if(!(in & 0x01)) led_shift(200);
	else if(!(in & 0x02)) led_all_on_off(200);
	else if(!(in & 0x04)) led_alternating(200);
	
	PCIFR |= 0x04;
}

int main(void)
{
	cli();
	
	// INTn 이용
	//EICRA = 0x30; // INT2 rising edge
	//EICRB = 0x0F; // INT4, INT5 rising edge
	//EIMSK = 0x34; // INT2, INT4, INT5 MASK
	//EIFR = 0x34; // 처음에 flag 넣어준다.
	
	// PCINTn 이용
	PCICR = 0x04;
	PCIFR = 0x04;
	PCMSK2 = 0x07; // PCINT16, 17, 18
	
	
	/************************************************************************/
	
	
	PORTC = 0xFF;
	DDRC = 0xFF;
	
	
	/************************************************************************/

	
	// 인터럽트를 받기 위한 D, E 입력 설정
	//DDRD &= ~(0x04);
	//DDRE &= ~(0x30);
	
	// 인터럽트를 받기 위한 K 입력 설정
	DDRK &= ~(0x07);
	
	
	/************************************************************************/

	
	sei(); // enable global interrupt
	
	while(1) {
		// normal operation
		
		// 아래 주석 처리된 코드는 무시할 것.
		//if(int_status == 2) {		// ISR은 int_status에 값만 넣어주고 바로 나온다.
			//led_all_on_off(300);	// int_stauts에 맞는 실행
			//int_status = 0;			// 
		//}
		//else if(int_status == 4) {
			//led_shift(300);
			//int_status = 0;
		//}
		//else if(int_status == 5) {
			//led_alternating(300);
			//int_status = 0;
		//}
	}
	
}

