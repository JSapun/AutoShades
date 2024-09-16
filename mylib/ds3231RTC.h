/**
 * Copyright (c) 2024, Justin Sapun <sapun.justin@gmail.com>
 *
 * This is an ATmega328p library for the Real Time Clock DS3231 chip.   
 * 
 * Features:
 * - Keeps time without an external power supply
 * - Can provide hardware interrupts to wake the chip
 *
 * References:
 * - Based on Sergey Denisov's implementation: https://github.com/LittleBuster/avr-rtc3231/blob/master/rtc3231.h
 * - DS3231 datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf
 */

#include <stdio.h>

#define RTC_WADDR 0b11010000
#define RTC_RADDR 0b11010001

struct rtc_time{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
};

struct rtc_date{
	uint8_t wday;
	uint8_t day;
	uint8_t month;
	uint8_t year;
};

void ds3231_init(void);
void ds3231_read_time(struct rtc_time *time);
void ds3231_write_time(struct rtc_time *time);
