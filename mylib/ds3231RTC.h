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
#define RTC_AlarmCTRLAddr 0x0E
#define RTC_Alarms_On 0b00000111
#define Alarm1_Addr 0x07
#define Alarm2_Addr 0x0B
#define RTC_STATUS_ADDR 0x0F


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
void ds3231_write_alarm(uint8_t alarm, struct rtc_time *time);
void ds3231_read_alarm(uint8_t alarm, struct rtc_time *time);
void ds3231_stop_alarms(void);
void ds3231_clear_alarm(uint8_t alarm);
uint8_t DS3231_get_status_reg(void);
uint8_t DS3231_get_control_reg(void);