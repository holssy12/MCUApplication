#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD_9600 103
unsigned char send_char =0;
ISR(INT4_vect)
{
	cli();
	_delay_ms(20);
	send_char='A';
	EIFR|=0x10;
	sei();
}
ISR(INT5_vect)
{
	cli();
	_delay_ms(20);
	send_char='B';
	EIFR|=0x20;
	sei();
}
int main(void)
{
	cli();
	DDRE&=~(0x30);
	EICRB=0x0F;
	EIMSK=0x30;
	EIFR|=0x30;
	
	//set buad
	UBRR1H=(unsigned char)(BAUD_9600>>8);
	UBRR1L=(unsigned char) BAUD_9600;
	
	UCSR1B=(1<<TXEN1)|(1<<RXEN1); //TX RX set
	UCSR1C=(1<<UCSZ11)|(1<<UCSZ10)|(1<<UPM11)|(1<<UPM10); //8bit char,Odd
	sei();
	while(1)
	{
		if(send_char)
		{
			while(!(UCSR1A& 1<<UDRE1));
			UDR1=send_char;
			send_char=0;
		}
		
	}
}
