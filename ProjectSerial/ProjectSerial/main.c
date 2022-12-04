#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 16000000UL
#define FOSC  16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "mc_ws2812.h"

#define BAUD_9600 103
#define BUF_SZ 100
unsigned char buf[BUF_SZ];
unsigned int com_size = 0;

#define NEOPIXEL_ON 0x01
#define NEOPIXEL_OFF 0x02
#define NEOPIXEL_BRIGHTNESS 0x04
#define NEOPIXEL_R 0x06
#define NEOPIXEL_G 0x08
#define NEOPIXEL_B 0x10
#define STX 0x02
#define ETX 0x03

#define LED_COUNT 16

/*
	0 : normal, 1 : data reception, 2 : Termination state
*/
int decode = 0;
int sint_flag = 0;
int _index = 0;

int _brightness = 128;	// 현재 밝기를 저장. 초기값 128.
int is_led_on = 0;		// 밝기를 조절할 때, 네오픽셀의 led가 1개 이상 켜져있었다면
						// 바로 업데이트를 해주기 위한 변수.
						
uint8_t _command = 0;	// 이번에 수행할 command를 저장.

rgbw_color redColor = (rgbw_color) { 0, 128, 0, 0 };
rgbw_color blueColor = (rgbw_color) { 0, 0, 128, 0 };
rgbw_color greenColor = (rgbw_color) { 128, 0, 0, 0 };
rgbw_color whiteColor = (rgbw_color) { 0, 0, 0, 128 };
rgbw_color selectedColor = (rgbw_color) { 0, 0, 0, 128 };	// 가장 최근에 선택된 color를 저장.
															// 초기값은 white.
	
rgbw_color currentColors[LED_COUNT] = { 0, };	// 네오픽셀의 모든 led를 관리하는 배열.
												// 초기값은 모두 0.

void hex_command(uint8_t cd);

/************************************************************************/

// 밝기를 업데이트 해주는 함수.
void update_brightness(rgbw_color* _color) {
	if(_color->red > 0) {
		_color->red = _brightness;
	}
	if(_color->blue > 0) {
		_color->blue = _brightness;
	}
	if(_color->green > 0) {
		_color->green = _brightness;
	}
	if(_color->white > 0) {
		_color->white = _brightness;
	}
}

// selectedColor에 담긴 밝기와 색깔로
// currentColors의 led를 전부 켜준다.
void led_all_on() {
	
	for(unsigned char i=0; i<LED_COUNT; i++) {
		currentColors[i] = selectedColor;
	}
	ws2812_setleds_rgbw_PB(6, currentColors, LED_COUNT);
}

// currentColors의 모두 0으로 만들어
// led가 꺼지게 한다.
void led_all_off() {
	
	for(unsigned char i=0; i<LED_COUNT; i++) {
		currentColors[i] = (rgbw_color) { 0, 0, 0, 0 };
	}
	ws2812_setleds_rgbw_PB(6, currentColors, LED_COUNT);
}

// index번째 currentColors 요소를
// color의 색깔과 밝기로 바꿔준다.
void led_color(rgbw_color color, int index){
	
	for(int i=0; i<LED_COUNT; i++) {
		if(i == index) currentColors[i] = color;
	}
	
	ws2812_setleds_rgbw_PB(6, currentColors, LED_COUNT);
}

// 밝기를 _value로 세팅해주는 함수.
void set_brightness(unsigned int _value) {
	
	_brightness = _value;
	
	// 각각 색깔들의 밝기를 모두 업데이트.
	update_brightness(&redColor);
	update_brightness(&greenColor);
	update_brightness(&blueColor);
	update_brightness(&whiteColor);
	update_brightness(&selectedColor);
	
	// 밝기를 업데이트했는데, 네오픽셀의 led가 1개 이상 켜진 상태였다면
	// 바로 업데이트 되도록 한다.
	if(is_led_on == 1) {
		for(int i=0; i<LED_COUNT; i++) {
			if(currentColors[i].red > 0) {
				currentColors[i] = redColor;
			}
			if(currentColors[i].green > 0) {
				currentColors[i] = greenColor;
			}
			if(currentColors[i].blue > 0) {
				currentColors[i] = blueColor;
			}
			if(currentColors[i].white > 0) {
				currentColors[i] = whiteColor;
			}
		}
		ws2812_setleds_rgbw_PB(6, currentColors, LED_COUNT);
	}
}

/************************************************************************/

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

/************************************************************************/

ISR(USART1_RX_vect) {
	cli();
	
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR1;
	if(decode == 0 && _usart_rcv == STX) {
		decode = 1;
	}
	else if(decode == 1) {
		_command = _usart_rcv;
		decode = 2;
	}
	else if(decode == 2) {
		// _commnd가 NEOPIXEL_R/G/B일 때, _usart_rcv에 들어오는 값은 네오픽셀의 led index이다.
		// 정상적인 값이라면 _index에 _usart_rcv값을 넣어주지만,
		// index가 0 ~ 15 사이가 아닌 경우에는, -1을 넣어준다.
		if(_command == NEOPIXEL_R || _command == NEOPIXEL_G || _command == NEOPIXEL_B) {
			if(_usart_rcv < LED_COUNT) _index = _usart_rcv;
			else _index = -1;
		}
		// _command가 NEOPIXEL_BRIGHTNESS일 때, _usart_rcv값은 밝기이다.
		else if(_command == NEOPIXEL_BRIGHTNESS) {
			_brightness = _usart_rcv;
		}
		
		decode = 3;
	}
	else if(decode == 3) {
		if(_usart_rcv == ETX) {
			sint_flag = 1;
		}
		else {
			myPrintf("RX error\n");
		}
		decode = 0;
	}
	
	sei();
}

void hex_command(uint8_t cd) {
	
	// _index가 -1인 경우는 0 ~ 15 범위를 넘어갔다는 것이므로,
	// 에러 메시지를 출력하고 _index를 다시 0으로 바꾸고 리턴한다.
	if(_index == -1) {
		myPrintf("Invalid index\n");
		_index = 0;
		return;
	}
	
	// cd 값에 따른 수행을 정해준다.
	//
	// NEOPIXEL_ON과 R/G/B는 led를 키므로 is_led_on을 1로 바꿔주는데,
	// R/G/B는 추가적으로 마지막 선택된 색을 나타내는 selectedColor를 업데이트해준다.
	//
	// NEOPIXEL_OFF : 모든 led를 끄고, is_led_on을 0으로 바꿔준다.
	//
	// NEOPIXEL_BRIGHTNESS : 밝기를 업데이트 해준다.
	//
	// 그 밖의 경우인 else는 에러 메시지를 출력해준다.
	if(cd == NEOPIXEL_ON) {
		is_led_on = 1;
		led_all_on();
	}
	else if(cd == NEOPIXEL_OFF) {
		is_led_on = 0;
		led_all_off();
	}
	else if(cd == NEOPIXEL_R) {
		is_led_on = 1;
		led_color(redColor, _index);
		selectedColor = redColor;
	}
	else if(cd == NEOPIXEL_G) {
		is_led_on = 1;
		led_color(greenColor, _index);
		selectedColor = greenColor;
	}
	else if(cd == NEOPIXEL_B) {
		is_led_on = 1;
		led_color(blueColor, _index);
		selectedColor = blueColor;
	}
	else if(cd == NEOPIXEL_BRIGHTNESS) {
		set_brightness(_brightness);
	}
	else {
		myPrintf("Invalid Command\n");
	}
}

/************************************************************************/

int main(void)
{
    /* Replace with your application code */
	
	cli();
	DDRE &= ~(0x30);
	EICRB = 0x0F;
	EIMSK = 0x30;
	EIFR |= 0x30;
	
	UBRR1H = (unsigned char) (BAUD_9600 >> 8);
	UBRR1L = (unsigned char) BAUD_9600;
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1B |= (1<<RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	sei();
	
	DDRB |= 0x01 << 6;
	
    while (1)
    {	
		if(sint_flag) {
			hex_command(_command);
			sint_flag = 0;
			_command = 0;
		}
    }
}

