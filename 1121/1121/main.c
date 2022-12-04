/*
 * 1121.c
 *
 * Created: 2022-11-21 오전 10:29:46
 * Author : holss
 */ 

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

#define MAX_TXBUF_SIZE 128
unsigned char tx_buf[MAX_TXBUF_SIZE];
int buf_len = 0;

#include <stdio.h>
#include <string.h>

void tx_char(unsigned char txChar) {
	while(!(UCSR1A & 1<<UDRE1)); // 블락킹
	UDR1 = txChar;
}

void tx_str(unsigned char *txStr, int len) {
	int i = 0;
	for(i=0; i<len; i++) {
		tx_char(txStr[i]);
	}
}

ISR(INT4_vect) {
	cli();
	_delay_ms(100);
	unsigned char *tmp;
	tmp = malloc(30);
	memset(tmp, '\0', 30);
	tmp = "INT4 PUSHED\n";
	buf_len = strlen(tmp);
	strncpy(tx_buf, tmp, buf_len);
	EIFR |= 0x10;
	sei();
}

ISR(INT5_vect) {
	cli();
	_delay_ms(100);
	unsigned char *tmp;
	tmp = malloc(30);
	memset(tmp, '\0', 30);
	tmp = "INT5 PUSHED\n";
	buf_len = strlen(tmp);
	strncpy(tx_buf, tmp, buf_len);
	EIFR |= 0x20;
	sei();
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
	
	myPrintf("*******************************************************\n");
	
	myPrintf("AVR SYSTEM init OK...\n");
	
	myPrintf("Clock Speed = %ld\n", F_CPU);
	
	myPrintf("Interrupts = 0x%x\n", EIMSK);
	
	myPrintf("*******************************************************\n");
	
    while (1) 
    {
		if(buf_len > 0) {
			tx_str(tx_buf, buf_len);
			memset(tx_buf, '\0', MAX_TXBUF_SIZE);
			buf_len = 0;
		}
		
		
    }
}



