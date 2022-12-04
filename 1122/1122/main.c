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

unsigned int sint_flag = 0;

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

ISR(USART1_RX_vect) {
	cli();
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR1;
	if(_usart_rcv == 0x0A || _usart_rcv == 0x0D || com_size >= BUF_SZ) {
		sint_flag = 1;
	}
	else {
		buf[com_size++] = _usart_rcv;
	}
	sei();
}

void command(uint8_t *buff, uint8_t len) {
	uint8_t tmp[BUF_SZ];
	uint8_t ret = 0;
	memset(tmp, '\0', BUF_SZ);
	memcpy((uint8_t*)tmp, (uint8_t*)buff, len);
	if(strstr((char*)tmp, "LED_ON_OFF") != NULL) {
		tx_str(buf, com_size);
		led_all_onoff(100);
	}
	else if (strstr((char*)tmp, "LED_ALT") != NULL){
		tx_str(buf, com_size);
		led_alternating(100);
	}
	else if (strstr((char*)tmp, "LED_SHIFT") != NULL){
		tx_str(buf, com_size);
		led_shift(100);
	}
	else {
		//tx_str(buf, com_size);
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
		// led_all_onoff(200);
		
		if(sint_flag) {
			buf[com_size] = '\0';
			command(buf, com_size);
			com_size = 0;
			memset(buf, '\0', 100);
			sint_flag = 0;
		}
    }
}

