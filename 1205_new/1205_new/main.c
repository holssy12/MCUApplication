#include <avr/io.h>
#define F_CPU 16000000UL
#define FREQ(x)(unsigned int) (16000000/(2*8(1+x)))
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define BAUD_9600 103

#define ON 1
#define OFF 2
int on_off = 0;
void play(unsigned int y)
{
	unsigned int x;
	DDRB|=0x02;
	TCCR1A|=0x04;
	TCCR1B|=0x1A;
	ICR1=y;
}
void stop(void)
{
	TCCR1A=0x00;
	TCCR1B=0x00;
	TCNT1=0;
	ICR1=0;
	DDRB=~0x02;
}

ISR(INT0_vect)
{
	cli();
	on_off=ON;
	EIFR|=0x01;
	sei();
}
ISR(INT1_vect)
{
	cli();
	on_off=OFF;
	EIFR|=0x02;
	sei();
}
int main(void)
{
	cli();
	UBRR1H=(unsigned char)(BAUD_9600>>8);
	UBRR1L=(unsigned char)BAUD_9600;
	UCSR1A=0x00;
	UCSR1B=(1<<TXEN1)|(1<<RXEN1);
	UCSR1B|=(1<<RXCIE1);
	UCSR1C=(1<<UCSZ11)|(1<<UCSZ10);
	DDRB=0x80;
	DDRD=0x00;
	PORTD=0x00;
	PORTB=0x00;
	EICRA=0x0A;
	EIMSK=0x03;
	EIFR|=0x03;
	sei();
	while(1){
		if(on_off ==ON){
			play(2273);
			on_off=0;
		}
		else if(on_off==OFF)
		{
			stop();
			on_off=0;
		}
	}
}
