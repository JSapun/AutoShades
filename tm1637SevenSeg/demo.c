/* This file contains an easy implementation for using the TM1637 driver.
 * The pins are hardcoded in the c header file. 
 * DIO - PORTC0
 * CLK - PORTC1
 * VDD - 5V
 * GND - GND
*/

#include <TM1637SevenSeg.h>

int main(void ){
  TM1637_init(1, 1); // Enable, Brightness
  TM1637_display_colon(1);
  TM1637_display_digit(0, 1); // Position, Value
  TM1637_display_digit(1, 2);
  TM1637_display_digit(2, 3);
  TM1637_display_digit(3, 7);
}