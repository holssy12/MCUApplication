#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "mc_ws2812.h"

#define BAUD_9600 103

#define LED_COUNT 16

rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };

rgbw_color currentColor[LED_COUNT] = {0, };
	
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
	
unsigned int GetADCData(unsigned aIn) {
	
	volatile unsigned int result;
	
	ADMUX = aIn;
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << ADLAR);
	
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADSC);
	
	while(!(ADCSRA & (1<<ADIF)));
	_delay_us(300);
	
	//result = ADCL + (ADCH << 8);
	result = (ADCL >> 6) + (ADCH << 2);
	ADCSRA = 0x00;
	
	myPrintf("ADC : %d\n", result);
	
	return result;
}

void neo_shift(unsigned int _value) {
	
	for(int i=0; i<LED_COUNT; i++) {
		
		if(i <= (_value / 64)) currentColor[i] = blueColor;
		else currentColor[i] = (rgbw_color) {0,0,0,0};
		
	}
	
	ws2812_setleds_rgbw_PB(4, currentColor, LED_COUNT);
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
		unsigned int adc_value = GetADCData(4);
		neo_shift(adc_value);
		
		_delay_ms(100);
    }
}

