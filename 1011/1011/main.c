/*
 * 1011.c
 *
 * Created: 2022-10-11 오전 9:26:12
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__	// _delay_ms 함수 parameter 해결법
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

//void delay_ms(uint16_t ms) {
	//while(ms--) {
		//_delay_ms(1);
	//}
//}


void led_all_onoff(int _n, int _s) {

	for (int i = 0; i < _n; i++){
		PORTC = 0x00;	// 전체 led on
		_delay_ms(_s);
		PORTC = 0xFF;
		_delay_ms(_s);	// 전체 led off
	}
}

void alternate_led_onoff(void) {
	for (int i = 0; i < 3; i++) {
		PORTC = 0xAA;
		_delay_ms(500);
		PORTC = 0x55;
		_delay_ms(500);
	}
	for (int i = 0; i < 4; i++) {
		PORTC = 0b11110000;
		_delay_ms(500);
		PORTC = 0b00001111;
		_delay_ms(500);
	}
	for (int i = 0; i < 5; i++) {
		PORTC = 0b11000011;
		_delay_ms(500);
		PORTC = 0b00111100;
		_delay_ms(500);
	}
}

void alternate_led_onoff_with_parameter(unsigned char _n, unsigned int _loop, unsigned int _s) {
	unsigned char new = (0xFF) ^ (_n);
	for (int i = 0; i < _loop; i++) {
		PORTC = _n;
		_delay_ms(_s);
		PORTC = new;
		_delay_ms(_s);
	}
}

//void 1s_complement (void){
	//1 2 4 8 10 20 40 80
	//PORTC = 0x01;
	//_delay_ms(100);
	//PORTC = 0x02;
	//_delay_ms(100);
	//PORTC = 0x02;
	//_delay_ms(100);
	//PORTC = 0x02;
	//_delay_ms(100);
	//PORTC = 0x02;
	//_delay_ms(100);
//}

void one_s_complement_after_shift(void) {
	unsigned int n = 0x01;
	
	for(int i=0; i<8; i++) {
		PORTC = ~(0x01 << i);
		_delay_ms(100);
	}
	
	for(int i=0; i<8; i++) {
		PORTC = ~(0b10000000 >> i);
		_delay_ms(100);
	}
}

void one_s_complement_after_shift_new(int _sh, int _s) {
	unsigned int n = 0x01;
	
	for (int j=0; j<8; j++) {
		for(int i=0; i<_sh; i++) {
			PORTC = ~(0x01 << i);
			_delay_ms(_s);
		}
		
		for(int i=0; i<_sh; i++) {
			PORTC = ~((0x01 << (_sh-1)) >> i);
			_delay_ms(_s);
		}	
	}
}

void project_1(int _sh) {
	
	while(1) {
		
		// LED_ALL_ON_OFF
		for (int i=0; i<3; i++) {
			PORTC = 0x00;	// 전체 led on
			PORTA = 0x00;
			_delay_ms(500);
			PORTC = 0x0F;
			PORTA = 0xF0;
			_delay_ms(500);	// 전체 led off
		}
		
		// LED_ALTERNATING
		for (int i = 0; i < 3; i++) {
			PORTC = 0x0A;
			PORTA = 0xA0;
			_delay_ms(500);
			PORTA = 0x50;
			PORTC = 0x05;
			_delay_ms(500);
		}
		for (int i = 0; i < 3; i++) {
			PORTC = 0x00;
			PORTA = 0xF0;
			_delay_ms(500);
			PORTC = 0x0F;
			PORTA = 0x00;
			_delay_ms(500);
		}
		for (int i = 0; i < 3; i++) {
			PORTC = 0x03;
			PORTA = 0xC0;
			_delay_ms(500);
			PORTC = 0b0C;
			PORTA = 0b30;
			_delay_ms(500);
		}
		
		// LED_SHIFT
		for (int j=0; j<5; j++) {
			for(int i=0; i<_sh; i++) {
				PORTC = ~(0x01 << i);
				PORTA = PORTC;
				_delay_ms(500);
			}
			for(int i=0; i<_sh; i++) {
				PORTC = ~((0x01 << (_sh-1)) >> i);
				PORTA = PORTC;
				_delay_ms(500);
			}
		}
			
	}
	
	
	
}

int main(void) {
	DDRC=0x0F;
	DDRA=0xF0;
	// led_all_onoff(3, 300);
	// alternate_led_onoff();
	// alternate_led_onoff_with_parameter(0x55, 3, 500);
	// one_s_complement_after_shift();
	// one_s_complement_after_shift_new(6,100);
	project_1(8);
}

