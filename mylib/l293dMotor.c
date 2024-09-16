/**
 * Copyright (c) 2024, Justin Sapun <sapun.justin@gmail.com>
 *
 * This is a custom l293d motor driver for working with atmega328p registers. Ports in header file.
 * 
 *
 * References:
 * - Datasheet: https://www.ti.com/lit/ds/symlink/l293.pdf
 * - Setup: https://europe1.discourse-cdn.com/arduino/original/4X/d/f/b/dfbd0642e3c6eadf71029c3b86e17112fcbada00.jpeg
 */


#include "l293dMotor.h"
#include <avr/io.h>

void l293d_init(void){
	// Set pins as output
	DDRD |= (1 << DDD2);
	DDRD |= (1 << DDD3);
	DDRD |= (1 << DDD4);
}

void l293d_clockwise(void){
	PORTD &= ~(1 << L293D_1A); // High for CC
	PORTD |= (1 << L293D_2A); // High for C
}

void l293d_counterclockwise(void){
	PORTD |= (1 << L293D_1A); // High for CC
	PORTD &= ~(1 << L293D_2A); // High for C
}

void l293d_stop(void){
	PORTD &= ~(1 << L293D_EN);
}

void l293d_start(uint8_t duration){
	PORTD |= (1 << L293D_EN); // Enable bit
}