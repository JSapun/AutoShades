/* This file contains an easy implementation for detecting low voltage cutoff for lipo batteries.
 * This method uses internal components inside avr chips to compare the internal 1.1v reference and
 * and ADC output to calculate vcc.
 *
 * Resources: https://github.com/cygig/MCUVoltage/tree/main
*/


#include <avr/io.h>
#include <util/delay.h>
#include <USARTE28.h>   
#include <ioE28.h> 
#include <battVCC.h>

// Arduino rev3 board Chip 1
// Bandgap: 993mV
// VCC: 4.60V

int main(void){
    USART_Init();
    batt_ADC_init();
	
    while(1){
      uint16_t vcc = get_avg_batt();
      myprintf("VCC (mV): %d\n\r", vcc);
		  _delay_ms(1000);			// Pause before next round			
	}
}