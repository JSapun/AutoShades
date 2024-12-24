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


#include <ioE28.h>
#include <USARTE28.h>

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
	uint8_t send1 = DS3231_get_control_reg() | RTC_Alarms_On; // Turn alarms on
	uint8_t send2 = DS3231_get_status_reg() & ~(0b10001000); // Turn OSF & 32kHz off

	i2c_start_condition(); 
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_AlarmCTRLAddr);
	i2c_send_byte(send1); 
	i2c_stop_condition();

	i2c_start_condition(); 
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_STATUS_ADDR);
	i2c_send_byte(send2); 
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

void ds3231_write_alarm(uint8_t alarm, struct rtc_time *time){
	i2c_start_condition();
	i2c_send_byte(RTC_WADDR);
	if (alarm != 1){
		i2c_send_byte(Alarm2_Addr);
	}
	else{
		i2c_send_byte(Alarm1_Addr);
		i2c_send_byte(bin(time->sec)); // Only alarm 1 has seconds
	}
	i2c_send_byte(bin(time->min));
    i2c_send_byte(bin(time->hour));
	i2c_send_byte(0b10000000); // To set bit mask for bit 7 (msb) for 4 alarm registers (when it will match).
    i2c_stop_condition();
}  

void ds3231_read_alarm(uint8_t alarm, struct rtc_time *time){
	i2c_start_condition();
	i2c_send_byte(RTC_WADDR);
	if (alarm != 1){
		i2c_send_byte(Alarm2_Addr);
	}
	else{
		i2c_send_byte(Alarm1_Addr);
	}
	i2c_stop_condition();

	i2c_start_condition();
	i2c_send_byte(RTC_RADDR);
	if (alarm == 1){
		time->sec = bcd(i2c_recv_byte()); // Only alarm 1 has seconds
	}
	time->min = bcd(i2c_recv_byte());
	time->hour = bcd(i2c_recv_byte());
	i2c_stop_condition();
}

void ds3231_stop_alarms(void){
	uint8_t send1 = DS3231_get_control_reg() & ~(00000011); // Turn alarms off

	i2c_start_condition(); 
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_AlarmCTRLAddr);
	i2c_send_byte(send1); 
	i2c_stop_condition();
}

void ds3231_clear_alarm(uint8_t alarm){ 
	uint8_t reg_val;
	if (alarm==1){
		reg_val = DS3231_get_status_reg() & ~(0b00000001); // Check if alarm 1 flag is raised
	}
	else{
		reg_val = DS3231_get_status_reg() & ~(0b00000010); // Check if alarm 2 flag is raised
	}
    
	i2c_start_condition();
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_STATUS_ADDR);
	i2c_send_byte(reg_val);
	i2c_stop_condition();
}

uint8_t DS3231_get_status_reg(void){
	i2c_start_condition(); // First send address you want to read
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_STATUS_ADDR);
	i2c_stop_condition();

	i2c_start_condition(); // Then, read register
	i2c_send_byte(RTC_RADDR);
	uint8_t rv = i2c_recv_byte();
	i2c_stop_condition();
	return rv;
}

uint8_t DS3231_get_control_reg(void){
	i2c_start_condition(); // First send address you want to read
	i2c_send_byte(RTC_WADDR);
	i2c_send_byte(RTC_AlarmCTRLAddr);
	i2c_stop_condition();

	i2c_start_condition(); // Then, read register
	i2c_send_byte(RTC_RADDR);
	uint8_t rv = i2c_recv_byte();
	i2c_stop_condition();
	return rv;
}