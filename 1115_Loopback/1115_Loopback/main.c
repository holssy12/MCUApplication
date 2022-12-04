/*
 * 1115_Loopback.c
 *
 * Created: 2022-11-15 오전 10:05:11
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD_9600 103
unsigned char send_char =0;


int main(void)
{
    /* Replace with your application code */
	
	unsigned char rx_char = 0;
	cli();
	DDRE &= ~(0x30);
	EICRB = 0x0F;
	EIMSK = 0x30;
	EIFR |= 0x30;
	
	UBRR1H = (unsigned char) (BAUD_9600 >> 8); // TX1, RX1을 쓰기 때문에 UBRR1[H/L] 을 쓴다.
												// define된 숫자는 integer로 인식된다. ( 16 비트 )
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	sei();
	
    while (1) 
    {
		// RX (Blocking)
		while(!(UCSR1A & (1<<RXC1)));
		rx_char = UDR1;
		// TX
		while(!(UCSR1A & 1<<UDRE1));
		UDR1 = rx_char;
    }
}

