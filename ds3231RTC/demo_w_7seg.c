/* This file contains an easy implementation for using the DS3231 driver with the 7seg.
 * Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf
*/


#include <util/delay.h>
#include <i2c.h>
#include <ds3231RTC.h>
#include <tm1637SevenSeg.h>


int main(void ){
    USART_Init();
    i2c_init();
    TM1637_init(1, 1); // Enable, Brightness
    TM1637_display_digit(0, 9); // Position, Value
    TM1637_display_digit(1, 9);
    TM1637_display_digit(2, 9);
    TM1637_display_digit(3, 9);

    ds3231_init(); 
    _delay_ms(2000); 
 
    while(1){   
        struct rtc_time out = {0};
        i2c_reset(); // Clear I2C bus as it may not be well defined, reset all registers
        ds3231_read_time(&out);
        TM1637_display_digit(0, out.hour/10); // Position, Value
        TM1637_display_digit(1, out.hour%10);
        TM1637_display_digit(2, out.min/10);
        TM1637_display_digit(3, out.min%10);
        if (out.sec < 30){
            TM1637_display_colon(0);
        }
        else {
            TM1637_display_colon(1);
        }
        _delay_ms(1000);
    }

    return 0;
}