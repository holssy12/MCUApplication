/*
 * 1107.c
 *
 * Created: 2022-11-07 오전 10:25:58
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

#define LED_COUNT 16

#define interrupt_is_disabled

#include "mc_ws2812.h"

void led_on(rgbw_color color) {
	ws2812_setleds_rgbw_PB(6, &color, 1); // led 1개만 쓴다.
}

void led_all_off() {
	rgbw_color tempColors[LED_COUNT];
	
	for(unsigned char i=0; i<LED_COUNT; i++) {
		tempColors[i] = (rgbw_color) { 0, 0, 0, 0 };
	}
	ws2812_setleds_rgbw_PB(6, tempColors, LED_COUNT);
}

int main(void)
{
	
	DDRB |= 0x01 << 6;

	rgbw_color redColor = (rgbw_color) { 0, 128, 0, 0 };
	rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };
	rgbw_color greenColor = (rgbw_color) { 128, 0, 0, 0 };
	
	
    /* Replace with your application code */
    while (1) 
    {
		led_on(redColor);
		_delay_ms(330);
		led_on(greenColor);
		_delay_ms(330);
		led_on(blueColor);
		_delay_ms(330);
		//led_all_off();
		//_delay_ms(330);
    }
}

