/* This file contains an easy implementation for using the DS3231 motor.
 * The pins are hardcoded in the c header file:
 *
 * VCC1 (5V Logic Input) - 5V
 * VCC2 (HV Motor Input) - Vin
 * 1,2 EN (Motor enable) - PORTD2
 * 1A (Motor direction forward) - PORTD3
 * 2A (Motor direction reverse) - PORTD4
 * 1Y (Motor output 1)
 * 2Y (Motor output 2)
 * GND - GND 
 * 
 * Datasheet: https://www.ti.com/lit/ds/symlink/l293.pdf
 * Setup: https://europe1.discourse-cdn.com/arduino/original/4X/d/f/b/dfbd0642e3c6eadf71029c3b86e17112fcbada00.jpeg
*/


#include <l293dMotor.h>
#include <util/delay.h>


int main(void ){ 
  l293d_init();
  l293d_clockwise();
  l293d_start();
  _delay_ms(2000);
  l293d_stop();
  _delay_ms(1000);
  l293d_counterclockwise();
  l293d_start();
  _delay_ms(2000);
  l293d_stop();
 
  return 0; 
}