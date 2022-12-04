/*
 * 1108.c
 *
 * Created: 2022-11-08 오전 8:55:12
 * Author : holss
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>

#define LED_COUNT 16

#define interrupt_is_disabled

#include "mc_ws2812.h"

void led_on(rgbw_color* color, unsigned char _n) {
	ws2812_setleds_rgbw_PB(6, color, LED_COUNT);
}

void led_all_off(unsigned char _n) {
	rgbw_color tempColors[LED_COUNT];
	
	for(unsigned char i=0; i<LED_COUNT; i++) {
		tempColors[i] = (rgbw_color) { 0, 0, 0, 0 };
	}
	ws2812_setleds_rgbw_PB(_n, tempColors, LED_COUNT);
}

void led_on_solution(rgbw_color color, int index){
	rgbw_color cArr[LED_COUNT];
	
	for(int i=0; i<LED_COUNT; i++) {
		if(i == index) cArr[i] = color;
		else cArr[i] = (rgbw_color) {0,0,0,0};
	}
	
	ws2812_setleds_rgbw_PB(6, cArr, LED_COUNT);
}

void led_all_off_solution() {
	rgbw_color tempColors[LED_COUNT];
	
	for(unsigned char i=0; i<LED_COUNT; i++) {
		tempColors[i] = (rgbw_color) { 0, 0, 0, 0 };
	}
	ws2812_setleds_rgbw_PB(6, tempColors, LED_COUNT);
}

void led_shift(rgbw_color color) {
	rgbw_color cArr[LED_COUNT];
	
	cArr[0] = color;
	
	for(int i=1; i<LED_COUNT; i++) {
		cArr[i] = (rgbw_color) {0,0,0,0};
	}
	
	for(int i=0; i<LED_COUNT; i++) {
		
		if(i==0) {
			ws2812_setleds_rgbw_PB(6, cArr, LED_COUNT);
			_delay_ms(100);
			continue;
		}
		
		rgbw_color tempColor = cArr[i - 1];
		cArr[i - 1] = cArr[i];
		cArr[i] = tempColor;
		
		ws2812_setleds_rgbw_PB(6, cArr, LED_COUNT);
		_delay_ms(100);
	}

}

void led_ring_brightness() {
	rgbw_color cArr[LED_COUNT];
	int dir=0;
	for(int i=0; dir? i>=0 : i<25; dir? i-- : i++) {
		for(int j=0; j<LED_COUNT; j++) {
			cArr[j] = (rgbw_color) {i*10, 0, 0, 0};
		}
		ws2812_setleds_rgbw_PB(6,cArr,LED_COUNT);
		_delay_ms(40);
		if(i==24){
			dir=1;
		}
	}
}

void led_contest(rgbw_color* colors, int index) {
	rgbw_color cArr[LED_COUNT];
	int dir=0;
	for(int i=0; dir? i>=0 : i<25; dir? i-- : i++) {
		for(int j=0; j<LED_COUNT; j++) {
			cArr[j] = (rgbw_color) {i*10, 0, 0, 0};
		}
		if(i==24){
			dir=1;
		}
		
			rgbw_color temp;
		
			for(int j=0; j<LED_COUNT; j+=4) {
				cArr[j] = (rgbw_color) {i*10, 0, 0, 0};
			}
			for(int k=1; k<LED_COUNT; k+=4) {
				cArr[k] = (rgbw_color) {0, i*10, 0, 0};
			}
			for(int m=2; m<LED_COUNT; m+=4) {
				cArr[m] = (rgbw_color) {0, 0, i*10, 0};
			}
			for(int n=3; n<LED_COUNT; n+=4) {
				cArr[n] = (rgbw_color) {0, 0, 0, i*10};
			}
			ws2812_setleds_rgbw_PB(6,cArr,LED_COUNT);
			_delay_ms(40);
		
	}
}



int main(void)
{
	DDRB |= 0x01 << 6;

	rgbw_color Colors[LED_COUNT];
	
	rgbw_color redColor = (rgbw_color) { 0, 128, 0, 0 };
	rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };
	rgbw_color greenColor = (rgbw_color) { 128, 0, 0, 0 };
	
	Colors[0] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[1] = (rgbw_color) { 0, 128, 0, 0 };
	Colors[6] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[12] = (rgbw_color) { 128, 0, 0, 0 };
		
	Colors[2] = (rgbw_color) { 0, 128, 0, 0 };
	Colors[3] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[4] = (rgbw_color) { 128, 0, 0, 0 };
		
	Colors[5] = (rgbw_color) { 0, 128, 0, 0 };
	Colors[7] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[8] = (rgbw_color) { 128, 0, 0, 0 };
		
	Colors[9] = (rgbw_color) { 0, 128, 0, 0 };
	Colors[10] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[11] = (rgbw_color) { 128, 0, 0, 0 };
		
	Colors[13] = (rgbw_color) { 0, 128, 0, 0 };
	Colors[14] = (rgbw_color) { 0, 0, 128, 0 };
	Colors[15] = (rgbw_color) { 128, 0, 0, 0 };
		
    /* Replace with your application code */
    while (1) 
    {
		//led_on(Colors, 1);
		//_delay_ms(3000);
		//led_all_off(1);
		//_delay_ms(3000);
		
		//led_on_solution(redColor, 0);
		//_delay_ms(330);
		//led_on_solution(blueColor, 6);
		//_delay_ms(330);
		//led_on_solution(greenColor, 12);
		//_delay_ms(330);
		
		//led_shift(redColor);
		//led_shift(blueColor);
		//led_shift(greenColor);
		
		//led_ring_brightness();
	
		led_contest(Colors, 3);
    }
}
