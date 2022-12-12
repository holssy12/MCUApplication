/*
 * Test_1.c
 *
 * Created: 2022-12-08 오후 9:56:05
 * Author : holss
 */ 

#define __DELAY_BACKWARD_COMPATIBLE
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#define BAUD_19200 51

#define BUF_SZ 100
unsigned char buf[BUF_SZ];
unsigned int com_size = 0;

#define LED_ONOFF 0x01
#define LED_ALT 0x01
#define LED_SHIFT 0x01
#define STX 0x02
#define ETX 0x03

unsigned int GetADCData(unsigned char aIn) {
	
	volatile unsigned int result;
	ADMUX = aIn;
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	ADCSRA |= (1 << ADSC);
	
	while(!(ADCSRA & (1 << ADIF)));
	
	_delay_us(300);
	
	result = ADCL + (ADCH << 8);
	ADCSRA = 0x00;
	return result;
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

void disp_led(unsigned int adc) {
	PORTC = 0xFF;
	for(int i=0; i<=(adc/128); i++) {
		PORTC &= ~(0x01 << i);
	}
}

int main(void)
{
    /* Replace with your application code */
	
	UBRR1H = (unsigned char) (BAUD_19200 >> 8);
	UBRR1L = (unsigned char) BAUD_19200;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	UCSR1C |= (1 << USBS1);
	UCSR1C |= (1 << UPM11) | (1 << UPM10);
	
	
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	DDRK = 0xFF;
	PORTK = 0x00;
	
    while (1) 
    {
		unsigned int adc_value = GetADCData(0);
		disp_led(adc_value);
		_delay_ms(500);
		
		myPrintf("ADC: %d\n", GetADCData(0)); // myPrintf는 필수... 디버깅 용도로도 용이.
    }
}

