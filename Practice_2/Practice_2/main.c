#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#define Trigger_1A PORTC|=0x01 //초음파 트리거
#define Trigger_0A PORTC&=~0x01//초음파 트리거
#define Echo (PINC&0x04) //초음파 에코

volatile unsigned char data;
volatile int range;
volatile unsigned char sec=0,min=0;//날짜
volatile int flag=0;
void getEcho(void);
void pulse(void);
void control(void);
void getEcho(void)
{
	while(!Echo); // high가 될때까지 대기 //
	TCNT3=0x00;
	TCCR3B=0x02; // 카운터 시작, 8분주 = 0.5us //
	while(Echo); // low가 될때까지 대기 //
	TCCR3B=0x08; // 카운터 정지 //
	range=TCNT3/116; // Cm로 변경 //
}
void pulse(void)
{
	Trigger_1A; _delay_us(10); Trigger_0A; // 10uS 이거 줘야지 신호시작
	getEcho();
	if(range< 50) { PORTB = 0X01;}
	else {PORTB= 0X05;}
}
void control (void)
{
	data=UDR0; // udr-> data save or transmit
	if(data=='F')
	{
		PORTB=0×05;
		flag=0;
	}
	if(data==’S')
	{
		PORTB=0×00;
		flag=0;
	}
	if(data==’L')
	{
		PORTB=0×01;
		flag=0;
	}
	if(data==’R')
	{
		PORTB=0×04;
		flag=0;
	}
	if(data==’B')
	{
		PORTB=0x0A;
		flag=0;
	}
	if(data==’A')
	{
		flag=1;
	}
}
ISR(USART0_RX_vect)
{
	control();
}
ISR(TIMER1_OVF_vect) // uart
{
	cli();
	TCNT1H = 0xC2;
	TCNT1L = 0xF6;
	UCSR0C = (0<<UPM01)|(1<<UPM00)|(1<<UCSZ01)|(1<<UCSZ00); // 패리티 01로 바꿈
	UCSR0B = (1<<RXEN0)|(0<<TXEN0)|(1<<RXCIE0);
	UBRR0H = 0X00;
	UBRR0L = 0X67;
	PORTA^= 0X01;
	if(flag==1){
		sec++;
		if(sec==60){ sec=0; min++;
			if(min==60) min =0;
		}
		if(min <1)
		{pulse();}
		else PORTB =0X00;
	}
	else min=sec=0;
	sei();
}
int main(void)
{
	cli();
	DDRB = 0xff;
	DDRE = 0X02;
	DDRC = 0×01; // PC0 ouput Trigger, PC1 input Echo
	DDRD = 0XFF;
	DDRA = 0XFF;
	TCCR3A=0×00; TCCR3B=0×08; // CTC mode
	TIMSK=0×04;
	TCCR1A=0×00;
	TCCR1B=0×05;//prescale 1024
	TCNT1H=0xFF;//initialize
	TCNT1L=0xC0;
	sei();
	do
	{
	}while(1);
}