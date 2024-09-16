/* 
 * battVCC.h
 * Justin Sapun 
 * 09/09/2024
 * Module for reading vcc for the atmega328p chip using the 1.1v internal reference and ADC.
 */

void batt_ADC_init(void);
uint16_t batt_readVccVoltage(uint32_t multimeter);
uint16_t get_avg_batt(void);