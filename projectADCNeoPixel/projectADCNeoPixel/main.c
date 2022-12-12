#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "mc_ws2812.h"

#define BAUD_9600 103
#define BUF_SZ 100
unsigned char buf[BUF_SZ];
unsigned int com_size = 0;

#define LED_COUNT 16

rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };
	
rgbw_color currentColors[LED_COUNT] = {0, };	// 네오픽셀의 모든 led를 관리하는 배열.
												// 초기값은 모두 0.

// ADC : 10 bit resolution이므로, unsigned int에 담아야 한다.
// REFS0는 전압을 결정한다.
// ADIF : 끝났다는 걸 확인하기 위한 수단. 인터럽트가 없기 때문에 Blocking으로 처리.
// delay : datasheet 상에서 기다리라고 한다.
unsigned int GetADCData(unsigned char aIn) {
	
	volatile unsigned result;
	ADMUX = aIn; // Channel Selection ( 0 ~ 7 )
	ADMUX |= (1 << REFS0); // AVCC 옵션 (외부 공급 5V)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// ADC Enable, ADC prescaler : 128
	ADCSRA |= (1 << ADSC); // ADC Start Conversion
	while(!(ADCSRA & (1 << ADIF))); // ADIF is set when an ADC completes
	_delay_us(300); // 주의) 마이크로 단위이다.
	result = ADCL + (ADCH << 8);
	ADCSRA = 0x00;
	return result;
}

// _value parameter를 통해 ADC 값을 받아오는데,
// 네오픽셀의 LED가 16개이므로, 1024 / 16 = 64. 
// 즉, LED 하나 당 64의 범위로 접근하여
// 받아온 ADC 값을 64로 나눈 값까지의 네오픽셀의 led를 킨다.
// ( i는 int type이므로, 몫으로 항상 정수가 나온다. )
// 나머지 led는 모두 꺼준다.
void neo_shift(unsigned int _value){
	
	for(int i=0; i< LED_COUNT; i++) {
		if(i <= (_value / 64)) {
			currentColors[i] = blueColor;
		}
		else {
			currentColors[i] = (rgbw_color) {0, 0, 0, 0}; 
		}
	}
	
	ws2812_setleds_rgbw_PB(6, currentColors, LED_COUNT);
}

void tx_char(unsigned char txChar) {
	while(!(UCSR1A & 1<<UDRE1));
	UDR1 = txChar;
}

void tx_str(unsigned char *txStr, int len) {
	int i = 0;
	for(i=0; i<len; i++) {
		tx_char(txStr[i]);
	}
}

void myPrintf(char *fmt,...) {
	va_list arg_ptr;
	uint8_t i, len;
	char sText[128];
	
	/* text buffer */
	for (i=0; i<128; i++) {
		sText[i] = 0;
	}
	va_start(arg_ptr, fmt);
	vsprintf(sText, fmt, arg_ptr);
	va_end(arg_ptr);
	
	len = strlen(sText);
	for(i=0; i<len; i++) {
		tx_char(sText[i]);
	}
}


int main(void)
{
    /* Replace with your application code */
	
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
    while (1)
    {	
		unsigned int adc_value = GetADCData(0);	
		neo_shift(adc_value);
		
		_delay_ms(100);
		
		myPrintf("ADC : %d\n", adc_value);
    }
}

