#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#define BAUD_9600 103
#define BUF_SZ 100
unsigned char buf[BUF_SZ];
unsigned int com_size = 0;

#define LED_ONOFF 0x01
#define LED_ALT 0x02
#define LED_SHIFT 0x04
#define STX 0x02
#define ETX 0x03

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

void disp_led(unsigned int adc) {
	PORTC = 0xFF;
	for(int i=0; i<=(adc/128); i++) {
		PORTC &= ~(0x01 << i);
	}
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

/*
	Baud Rate를 바꾸는 거 연습해야 한다!! ( CatTerm에 떠있는 숫자 정도... )
	Stop bit 바꾸는 법도!!
	8N1 : 8bit에 stop bit 1개
*/

int main(void)
{
    /* Replace with your application code */
	
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	DDRC = 0xFF;
	PORTC = 0xFF;
	
    while (1)
    {	
		unsigned int adc_value = GetADCData(0);	
		disp_led(adc_value);
		_delay_ms(500);
		
		// myPrintf("ADC: %d\n", GetADCData(0)); // myPrintf는 필수... 디버깅 용도로도 용이.
    }
}

