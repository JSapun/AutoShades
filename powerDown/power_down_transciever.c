/*
 * Transciever drawing 6.6mA when transmitting 1 byte repetitively. Drawing 0.33mA when idle, and >1uA when powered off.
 * 
*/

#include <util/delay.h>
#include <nrf24l01.h>
#include <avr/io.h>


int main(void){
	DDRD |= (1 << PORTD5);
	PORTD |= (1 << PORTD5);
	nrf24_device(0, RESET); // Transmiter 
	uint8_t known_message = 0XAA;  
	for (uint16_t i = 0; i < 10000; i++){
    	while(nrf24_transmit(&known_message, 1, NO_ACK_MODE) == TRANSMIT_FAIL);
    	while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
  	}

	PORTD &= ~(1 << PORTD5);
	_delay_ms(5000);
	PORTD |= (1 << PORTD5);
	
	nrf24_device(3, RESET); // Turn off >1uA current draw
}