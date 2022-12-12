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
int note = 0;

unsigned int note_freq[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};

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
	x = FREQ(note_freq[y % 7]);
	ICR1 = x;
	on_off = 0;
	PORTC &= ~0x01;
}

void stop(void) {
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0;
	ICR1 = 0;
	DDRB = ~0x80;
	on_off = 0;
	PORTC |= 0x01;
}

ISR(INT0_vect) {
	cli();
	
	if(!ICR1) return;
	
	if(++note > 6) {
		note = 6;
	}
	EIFR |= 0x01;	
	
	on_off = ON;

	sei();
	myPrintf("01 Clicked \n");
	_delay_ms(100);
}

ISR(INT1_vect) {
	cli();
	if(!ICR1) return;
	
	if(--note < 0) {
		note = 0;
	}
	EIFR |= 0x02;
	
	on_off = ON;
	
	sei();
	myPrintf("02 Clicked \n");
	_delay_ms(100);
}

ISR(INT4_vect) {
	cli();
	if(ICR1) {
		on_off = OFF;
	}
	else {
		on_off = ON;
	}
	EIFR |= 0x10;
	sei();
	myPrintf("04 Clicked \n");
	_delay_ms(100);
}



int main(void)
{
	/* Replace with your application code */
	
	cli();
	DDRD = 0x00;
	DDRC = 0x01;
	DDRB = 0x00;
	PORTB = 0x00;
	EICRA = 0x0F;
	EICRB = 0x03;
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1A = 0x00;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1B |= (1 << RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	// Falling edge trigger
	EIMSK |= 0x13;
	// Interrupt enable
	EIFR |= 0x13;
	PORTC = 0x01;
	
	sei();
	
	stop(); // 잔여값 지우기?
	
	while (1)
	{
		if(on_off == ON) {
			play(note);
			_delay_ms(100);
		}
		else if(on_off == OFF) {
			stop();
			_delay_ms(100);
		}
	}
	
}

