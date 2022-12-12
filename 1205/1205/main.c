#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#define FREQ(x) (unsigned int)(16000000/(2*8*(1+x)))
#include <util/delay.h>
#define BAUD_9600 103
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define ON 1
#define OFF 2
int on_off = 0;

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

void play(unsigned int y) {
	unsigned int x;
	DDRB |= 0x80;
	TCCR1A |= 0x04;
	TCCR1B |= 0x1A;
	ICR1 = y;
}

void stop(void) {
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0;
	ICR1 = 0;
	DDRB &= ~0x80;
}

ISR(INT0_vect) {
	cli();
	on_off = ON;
	EIFR |= 0x01;
	sei();
	myPrintf("01 Clicked \n");
}

ISR(INT1_vect) {
	cli();
	on_off = OFF;
	EIFR |= 0x02;
	sei();
	myPrintf("02 Clicked \n");
}



int main(void)
{
    /* Replace with your application code */
	
	cli();
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1A = 0x00;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1B |= (1 << RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	DDRD = 0x00;
	DDRB = 0x80; // 7번 비트 활성화
	
	PORTD = 0x00;
	PORTB = 0x00;
	EICRA = 0x0A;
	
	// Falling endge trigger
	EIMSK |= 0x03;
	// Interrupt enable
	EIFR |= 0x03;
	
	sei();
	
    while (1) 
    {
		if(on_off == ON) {
			play(2273);
			on_off = 0;
		}
		else if(on_off == OFF) {
			stop();
			on_off = 0;
		}
    }
}

