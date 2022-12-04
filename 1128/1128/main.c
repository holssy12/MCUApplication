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

/*
	0 : normal, 1 : data reception, 2 : Termination state
*/
int decode = 0;
int sint_flag = 0;
uint8_t _command = 0;

void led_all_onoff(int _s) {
	
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

ISR(USART1_RX_vect) {
	cli();
	
	myPrintf("hello\n");
	
	
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR1;
	if(decode == 0 && _usart_rcv == STX) {
		decode = 1;
	}
	else if(decode == 1) {
		_command = _usart_rcv;
		decode = 2;
	}
	else if(decode == 2) {
		if(_usart_rcv == ETX) {
			sint_flag = 1;
		}
		else {
			myPrintf("RX error\n");
		}
		decode = 0;
	}
	
	sei();
}

void hex_command(uint8_t cd) {
	if(cd == LED_ONOFF) {
		led_all_onoff(100);
	}
	else if(cd == LED_ALT) {
		led_alternating(100);
	}
	else if(cd == LED_SHIFT) {
		led_shift(100);
	}
	else {
		myPrintf("Invalid Command\n");
	}
}

int main(void)
{
	/* Replace with your application code */
	
	cli();
	DDRE &= ~(0x30);
	EICRB = 0x0F;
	EIMSK = 0x30;
	EIFR |= 0x30;
	
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1B |= (1<<RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	sei();
	
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	while (1)
	{
		if(sint_flag) {
			hex_command(_command);
			sint_flag = 0;
			_command = 0;
		}
	}
}

