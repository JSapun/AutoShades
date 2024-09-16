/* This file contains an easy implementation for using the DS3231 driver without interrupt.
 * The pins are hardcoded in the c header file:
 * SCL - SCL
 * SDA - SDA 
 * VDD - 3.3V
 * GND - GND
 * SQW (Interrupt) - 
 * 
 * Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf
*/


#include <ioE28.h>
#include <USARTE28.h>
#include <util/delay.h>
#include <i2c.h>
#include <ds3231RTC.h>


int main(void ){
  USART_Init();
  i2c_init();
  ds3231_init();
  myprintf("Starting\n\r");

  // Set time 
  struct rtc_time time = {0};
  time.sec = 58;
  time.min = 59;
  time.hour = 23;
  ds3231_write_time(&time);
  myprintf("Set!\n\r");

  while(1){
    struct rtc_time out = {0};
    i2c_reset(); // Clear I2C bus as it may not be well defined, reset all registers
    ds3231_read_time(&out);
    myprintf("Hour: %d, Minute: %d, Second: %d\n\r", out.hour, out.min, out.sec);
    _delay_ms(1000);
  }

  return 0;
}