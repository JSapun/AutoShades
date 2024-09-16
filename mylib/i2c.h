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


#include <avr/io.h>


void i2c_init(void);
void i2c_uninit ( void );
void i2c_start_condition(void);
void i2c_stop_condition(void);
void i2c_send_byte(unsigned char byte);
/*void i2c_send_packet(unsigned char value, unsigned char address);*/
unsigned char i2c_recv_byte(void);
unsigned char i2c_recv_last_byte(void);