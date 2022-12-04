/*
 * 1017.c
 *
 * Created: 2022-10-17 오전 10:29:31
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>


//int main(void)
//{
	//unsigned char in;
	//unsigned char val=0x01;
	//
	//DDRC = 0xFF; // 원래는 DDRC &= 로 하는게 좋다.
	//// DDRL = ~(0x07); // PL0, PL1, PL2 INPUT
	//DDRL = 0xFE; // 마지막 비트만 INPUT으로 쓴다.
	//PORTC = ~val;
	//
	///* Replace with your application code */
	////while(1) {
		////in = (PINL & 0x07); // PL0, PL1, PL2 입력값 확인 ---> 어떤 게 눌리는 지 알 수 있다.
		////PORTC = in; // 입력받은 포트 L의 값을 포트 C(led)로 출력
	////}
	//
	//while(1) {
		//
		//in = (PINL & 0x01);
		//if (!in) { // 스위치가 눌리면 0
			//if (val & 0x80) val = 0x01;
			//else val = val << 1;
		//}
		//PORTC = ~val;
		//_delay_ms(300);
	//}
//}

int main(void)
{
// unsigned char in;
unsigned char val=0x01;

DDRC = 0xFF; // 원래는 DDRC &= 로 하는게 좋다.
// DDRL = ~(0x07); // PL0, PL1, PL2 INPUT
DDRL = 0xFC; // 마지막 비트만 INPUT으로 쓴다.
PORTC = ~val;

/* Replace with your application code */
//while(1) {
//in = (PINL & 0x07); // PL0, PL1, PL2 입력값 확인 ---> 어떤 게 눌리는 지 알 수 있다.
//PORTC = in; // 입력받은 포트 L의 값을 포트 C(led)로 출력
//}

while(1) {


	if ((PINL & 0x02)) {
		
		if(!(PINL & 0x01)) {
			if (val & 0x80) val = 0x01;
			else val = val << 1;
		}
		
	}
	else {
		if(!(PINL & 0x02)) {
			if (val & 0x01) val = 0x80;
			else val = val >> 1;
		}
	}
	PORTC = ~val;
	_delay_ms(300);

}
}


