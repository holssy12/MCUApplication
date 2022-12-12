#define __DELAY_BACKWARD_COMPATIBLE__
#include<avr/io.h>
#define FOSC 16000000UL
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

int norm_velocity = 125;
int current_LED_velocity = 125;

int inversed = 0;

int is_sw2_enabled = 0;
int is_sw3_enabled = 0;

unsigned char current_PK;
unsigned char current_PD;

int flag = 0;
int cnt = 0; // 125가 0인 상태.

ISR(PCINT2_vect) {
	_delay_ms(100);
	cli();
	PCIFR |= 0x04;
	sei();
	
	unsigned char in = PINK;
	
	if (in & 0x01) { // high -> low, 스위치를 눌렀다가 뗄 때.
		if (inversed == 0) {
			inversed = 1;
			PORTD = ~PORTD;
			PORTK = ~PORTK;
		}
		else if (inversed == 1) {
			inversed = 0;
			PORTD = ~PORTD;
			PORTK = ~PORTK;
		}
	}
}

ISR(INT4_vect) {
	_delay_ms(25);
	cli();
	EIFR |= 0x10;
	sei();
	
	unsigned char in = PINE & 0x10;
	
	if (!(in & 0x10)) { // 스위치가 눌렸을 때
		
		// 스위치 2번을 눌렀는데, 현재 스위치 3번이 눌려있는 상태라면
		// 일단 현재 상태를 기록해두었다가, 꺼준다.
		if (is_sw3_enabled == 1) {
			
			is_sw2_enabled = 1;
			
			current_PK = PORTK;
			current_PD = PORTD;
			
			PORTK = 0xFF;
			PORTD = 0xFF;
			
			if (cnt > 0 && cnt <= 8) { // cnt가 증가된 상태.
				current_LED_velocity -= norm_velocity;
				
				// 분명 초기 상태에서 실행하면,
				// LED 1번이 켜져야 하는데 왜 안켜지는가?
			}
			
			if (cnt >= 0 && cnt <= 3) {
				PORTD = (0x01 << cnt);
				PORTK = (0x00);
			}
			else if (cnt >= 4 && cnt <= 8) {
				PORTK = (0x01 << (cnt-1));
				PORTD = 0x00;
			}
			
			// 이전 main문으로 돌아가지 않도록 막기 위한 무한 루프
			while (1) {
				if (flag == 1) {
					
					if (cnt > 0 && cnt <= 8) {
						cnt--;
						
					}
					PORTK = current_PK;
					PORTD = current_PD;
					
					flag = 0;
					break;
				}
			}
		}
		// 스위치 2번을 눌렀는데, 현재 스위치 3번이 눌려있지 않은 상태라면
		// 그냥 enabled 되었다고만 판단하고, 딱히 아무 것도 해주지 않는다.
		// 다음 동작을 기다린다.
		else if (is_sw3_enabled == 0) {
			is_sw2_enabled = 1;
		}
	}
	else if (in & 0x10) { // 스위치가 떼졌을 때
		
		// 스위치를 뗐는데 스위치 3번이 눌린 상태라면,
		if (is_sw3_enabled == 1) {
			if (flag == 0) {
				flag = 1;
			}
			
			is_sw2_enabled = 0;
		}
		else if (is_sw3_enabled == 0) {
			is_sw2_enabled = 0;
		}
	}
}

ISR(INT5_vect) {
	_delay_ms(25);
	cli();
	EIFR |= 0x20;
	sei();
	
	unsigned char in = PINE & 0x20;
	
	if (!(in & 0x20)) { // 스위치가 눌렸을 때
		
		is_sw3_enabled = 1;
		
		// 스위치 2번을 눌렀는데, 현재 스위치 3번이 눌려있는 상태라면
		// 멈추고, 현재 속도 상태를 보여준다.
		if (is_sw2_enabled == 1) {
			current_PK = PORTK;
			current_PD = PORTD;
			
			PORTK = 0xFF;
			PORTD = 0xFF;
			
			if (cnt < 8 && cnt >= 0) { // cnt가 증가된 상태.
				current_LED_velocity += norm_velocity;
			}
			
			if (cnt >= 0 && cnt <= 3) {
				PORTD = ~(0x01 << cnt);
			}
			else if (cnt >= 4 && cnt <= 8) {
				PORTK = ~(0x01 << (cnt-1));
			}
			
			while (1) {
				if (flag == 1) {
					
					if (cnt >= 0 && cnt < 8) {
						cnt++;
					}
					
					PORTK = current_PK;
					PORTD = current_PD;
					
					flag = 0;
					break;
				}
			}
		}
		else if (is_sw2_enabled == 0) {
			is_sw3_enabled = 1;
		}
	}
	else if (in & 0x20) { // 스위치가 떼졌을 때
		
		if (is_sw2_enabled == 1) {
			if (flag == 0) {
				flag = 1;
			}
			
			is_sw3_enabled = 0;
		}
		else if (is_sw2_enabled == 0) {
			is_sw3_enabled = 0;
		}
	}
}


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

