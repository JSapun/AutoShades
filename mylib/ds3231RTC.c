/**
 * Copyright (c) 2024, Justin Sapun <sapun.justin@gmail.com>
 *
 * This is an ATmega328p library for the Real Time Clock DS3231 chip. This c file contains the low and high level source code. 
 * It uses i2c clocking and communication.
 * 
 * Features:
 * - Keeps time without an external power supply
 * - Can provide hardware interrupts to wake the chip
 *
 * References:
 * - Based on Sergey Denisov's implementation: https://github.com/LittleBuster/avr-rtc3231/blob/master/rtc3231.h
 * - DS3231 datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf
 */

#include "ds3231RTC.h"
#include "i2c.h"

static unsigned char bcd(unsigned char data){
	unsigned char bc;

	bc = ((((data & (1 << 6)) | (data & (1 << 5)) | (data & (1 << 4)))*0x0A) >> 4)
	+ ((data & (1 << 3))|(data & (1 << 2))|(data & (1 << 1))|(data & 0x01));

	return bc;
}

static unsigned char bin(unsigned char dec){
	char bcd;
	char n, dig, num, count;

	num = dec;
	count = 0;
	bcd = 0;

	for (n = 0; n < 4; n++) {
		dig = num % 10;
		num = num / 10;
		bcd = (dig << count) | bcd;
		count += 4;
	}
	return bcd;
}


void ds3231_init(void){
	i2c_start_condition();
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(0x0E);
	i2c_send_byte(0x20);
	i2c_send_byte(0x08);
	i2c_stop_condition();
}

void ds3231_read_time(struct rtc_time *time){
	i2c_start_condition();
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(0x00);
	i2c_stop_condition();

	i2c_start_condition();
	i2c_send_byte(RTC_RADDR);
	time->sec = bcd(i2c_recv_byte());
	time->min = bcd(i2c_recv_byte());
	time->hour = bcd(i2c_recv_byte());
	i2c_stop_condition();
}

void ds3231_write_time(struct rtc_time *time){
    i2c_start_condition();
    i2c_send_byte(RTC_WADDR);
    i2c_send_byte(0x00);
    i2c_send_byte(bin(time->sec));
	i2c_send_byte(bin(time->min));
    i2c_send_byte(bin(time->hour));
    i2c_stop_condition();
}