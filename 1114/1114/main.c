/*
 * 1114.c
 *
 * Created: 2022-11-14 오전 10:25:33
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

#define LED_COUNT 16

#define interrupt_is_disabled

#include "mc_ws2812.h"


int main(void)
{
	DDRB |= 0x01 << 6;
	
	/* Replace with your application code */
	while (1)
	{

	}
}

