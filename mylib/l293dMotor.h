/**
 * Copyright (c) 2024, Justin Sapun <sapun.justin@gmail.com>
 *
 * This is a custom l293d motor driver for working with atmega328p registers. 
 * 
 *
 * References:
 * - Datasheet: https://www.ti.com/lit/ds/symlink/l293.pdf
 * - Setup: https://europe1.discourse-cdn.com/arduino/original/4X/d/f/b/dfbd0642e3c6eadf71029c3b86e17112fcbada00.jpeg
 */


#define	L293D_EN				PORTD2
#define	L293D_1A				PORTD3
#define	L293D_2A				PORTD4


void l293d_init(void);
void l293d_clockwise(void);
void l293d_counterclockwise(void);
void l293d_stop(void);
void l293d_start(uint8_t duration);
