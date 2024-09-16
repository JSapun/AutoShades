/*
 * Fully functioning power down demo file. I was able to get the current consumption to 17uA from the barebones atmega328p.
 * When awake, the chip draws 8.5 mA roughly. Through trial and error, I was able to reduce the draw from 15mA to 17uA.
 * For the transmiter, the transciever can be tied to vcc but will need to go into power down mode, which draws >1uA. 
 * For a 3V battery with 150mAh capacity, device should theoretically be able to last 1 year. 
 * 
*/

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>


void initPinChangeInterrupt(void);

/* GLOBAL VARIABLES */
volatile uint8_t buttonPushed_D2 = 0;

int main (void)
{
	DDRB |= (1 << PORTB5);
	DDRD  &= ~(1 << DDD2);			    // D2 --> input (external interrupt)
	PORTD |= (1 << PORTD2);
	initPinChangeInterrupt();
	while(1){
		if (buttonPushed_D2 == 1){
			buttonPushed_D2 = 0;
			sleep_disable();
			//power_all_enable();
			PORTB |= (1 << PORTB5);
		}
		else{
			PORTB &= ~(1 << PORTB5);
			
			// disable ADC
			ADCSRA = 0;

			// turn off various modules
			power_all_disable ();
			
			set_sleep_mode (SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			
			cli();
			
			// turn off brown-out enable in software
			MCUCR |= (1 << BODS) | (1 << BODSE);
			MCUCR |= (1 << BODS);

			sei();
			sleep_cpu ();
		}
	}
}

ISR(INT0_vect) {
	buttonPushed_D2 = 1;            // Set flag to notify main
}
void initPinChangeInterrupt(void) {
	EICRA &= ~(1 << ISC01); // Set Low level behavior for INT0 (D2)
	EICRA &= ~(1 << ISC00);

	EIMSK |= (1 << INT0); // set External Interrupt Request to INT0 bit in EIMSK (D2)
}
