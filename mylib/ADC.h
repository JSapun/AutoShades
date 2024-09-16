/* 
 * ADC.h
 * Justin Sapun 
 * Lab 5 - 02/02/2023
 * Device driver header file for ADC onboard the Atmega328P.
 * Engs 28
 */

void ADC_setChannel(uint8_t channel);	// Set the ADC input pins (0-5)
void ADC_setReference(uint8_t Vref);	// Set the ADC voltage reference
void ADC_Init(void);					// Initialize the ADC
uint16_t ADC_getValue(void);			// Initiate conversion, return result

