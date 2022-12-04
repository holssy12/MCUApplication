/*
 * Project_NeoPixel_1.c
 *
 * Created: 2022-11-10 오후 12:25:04
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

#define LED_COUNT 16

#define interrupt_is_disabled

#include "mc_ws2812.h"

void led_symmetric_shift(rgbw_color color) {
	
	rgbw_color cArr[LED_COUNT];
	
	int dir = 1;
	
	for(int i=0; i<LED_COUNT; i++) {
		if(i == 0 || i == 15) {
			cArr[i] = color;
		}
		else {
			cArr[i] = (rgbw_color) {0, 0, 0, 0};
		}
	}
	
	for(int i=0; dir ? i < LED_COUNT / 2 : i > 0 ; dir ? i++ : i--) {
		
		if(dir == 1 && i==0) {
			ws2812_setleds_rgbw_PB(6, cArr, LED_COUNT);
			_delay_ms(100);
			continue;
		}
		
		int k, j;
		if(dir == 1) {
			k = i;
			j = LED_COUNT - i;
		}
		else {
			k = i + 1;
			j = LED_COUNT - i - 1; 
		}
		
		rgbw_color tempColor1 = cArr[k];
		cArr[k] = cArr[k - 1];
		cArr[k - 1] = tempColor1;
			
		
		rgbw_color tempColor2 = cArr[j];
		cArr[j] = cArr[j - 1];
		cArr[j - 1] = tempColor2;
		
		ws2812_setleds_rgbw_PB(6, cArr, LED_COUNT);
		_delay_ms(100);
		
		if(i == (LED_COUNT/2) - 1) { 
			dir = 0;
		}
	
	}

}

int main(void)
{
    /* Replace with your application code */
	
	DDRB |= 0x01 << 6;
	
	rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };
	
    while (1) 
    {
		led_symmetric_shift(blueColor);
    }
}

