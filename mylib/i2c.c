/**
 * Copyright (c) 2024, Justin Sapun <sapun.justin@gmail.com>
 *
 * This is a custom i2c library for managnig atmega328p registers which operate the two wire communication. 
 * I2c uses a clocking and data line.
 * 
 *
 * References:
 * - Based on Sergey Denisov's implementation: https://github.com/LittleBuster/avr-i2c
 * - i2c datasheet: https://www.nxp.com/docs/en/user-guide/UM10204.pdf
 */


#include "i2c.h"


void i2c_init(void){
	TWBR = 0xFF;
}

void i2c_reset(void){
    TWCR &= ~(1 << TWEN);
}

static void i2c_wait(void){
	while ((TWCR & (1 << TWINT)) == 0);
}

void i2c_start_condition(void){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait();
}

void i2c_stop_condition(void){
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_send_byte(unsigned char byte){
	TWDR = byte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
}

/*void i2c_send_packet(unsigned char value, unsigned char address){
	i2c_start_condition();
	i2c_send_byte(address);
	i2c_send_byte(value);
	i2c_stop_condition();
}*/

unsigned char i2c_recv_byte(void){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    i2c_wait();
    return TWDR;
}

unsigned char i2c_recv_last_byte(void){
	TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
    return TWDR;
}