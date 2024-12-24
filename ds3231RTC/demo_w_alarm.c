/* This file contains an easy implementation for using the DS3231 driver with an interrupt and alarms.
 * The pins are hardcoded in the c header file:
 * SCL - SCL
 * SDA - SDA 
 * VDD - 3.3V
 * GND - GND
 * SQW (Interrupt) - D2
 * 
 * LED for testing - D4
 * 
 * Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf
*/

#include <util/delay.h>


#include <avr/io.h>
#include <avr/interrupt.h> 
#include <ioE28.h>

#include <i2c.h>
#include <ds3231RTC.h>
#include <tm1637SevenSeg.h>

#include <ioE28.h>
#include <USARTE28.h>


void get_reg(void);

void initPinChangeInterrupt(void);
volatile uint8_t buttonPushed_D2 = 0;


int main(void ){
    initPinChangeInterrupt();
    USART_Init();
    i2c_init();
    ds3231_init();

    DDRD &= ~(1<<PORTD2);
    PORTD |= (1<<PORTD2); // Set internal pullup
    DDRD |= (1 << PORTD4);

    // set time
    struct rtc_time time = {0};
    time.sec = 50;
    time.min = 19;
    time.hour = 23;
    ds3231_write_time(&time);

    /* 1.) Set Alarm 1 for 10s */
    struct rtc_time rtc_set_a1 = {0};
    rtc_set_a1.hour = 23;
    rtc_set_a1.min = 20;
    rtc_set_a1.sec = 0; 
    
    ds3231_clear_alarm(1);
    ds3231_clear_alarm(2); 
    ds3231_write_alarm(1, &rtc_set_a1);

    while(1) {
        if (buttonPushed_D2==1){
            PORTD |= (1 << PORTD4);
            buttonPushed_D2 = 0;
            myprintf("Done_interrupt!\n\r");
        }

        if ((DS3231_get_status_reg() & 0b00000001)!=0){ // bit is set
            myprintf("Done\n\r");
            //ds3231_clear_alarm(1);
        }
        get_reg(); // Read registers manually
        _delay_ms(1000);
    }
    return 0;  
}

ISR(INT0_vect) {
	buttonPushed_D2 = 1;            // Set flag to notify main
}
void initPinChangeInterrupt(void) {
	//EICRA &= ~(1 << ISC01); // Set Low level behavior for INT0 (D2)
    EICRA &= ~(1 << ISC00);
    EICRA |= (1 << ISC01);

	EIMSK |= (1 << INT0); // set External Interrupt Request to INT0 bit in EIMSK (D2)
    sei();
}

void get_reg(void){
    myprintf("control: %d, status: %d\n\r", DS3231_get_control_reg(), DS3231_get_status_reg());
}