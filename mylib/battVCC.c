/* 
 * battVCC.c
 * Justin Sapun 
 * 09/09/2024
 * Module for reading vcc for the atmega328p chip using the 1.1v internal reference and ADC.
 */


#include <avr/io.h>		// All the port definitions are here
#include <util/delay.h>
#include "battVCC.h"		 


void batt_ADC_init(void){
    ADMUX |= 0b01001110;                                // Set to 1.1v reference
    ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2);   // Prescale by 128 (by default)
	ADCSRA |= (1 << ADEN);  						    // Enable ADC
}

uint16_t batt_readVccVoltage(uint32_t bandgap) {     // Returns vcc in mV
    ADCSRA |= (1 << ADSC); 				             // Start conversion
    while ((ADCSRA & (1 << ADSC)) != 0){}            // Wait for completion
    uint16_t value = ADC;                            // Read result
    uint16_t vcc = (bandgap * 1024) / value;         // Calculate vcc based on varying bandgap
    return vcc;

    /* To calculate bandgap, you need to first run this function with bandgap set to 1100. Then note the vcc output.
    Using regular scaling bandgap_new/bandgap_old = vcc_new/vcc_old, get bandgap_new and use it as the new input.*/
}

uint16_t get_avg_batt(void) {     // Returns vcc in mV
    uint32_t val = 0;
    for(uint8_t i = 0; i < 20; i++){
        val += batt_readVccVoltage(993); // Bandgap set to 993mV
        _delay_ms(5);
    }
    val /= 20;
    return (uint16_t) val;
}


