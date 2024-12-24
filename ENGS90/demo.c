#include <avr/io.h>	
#include <util/delay.h>


int main(void){
  //DDRB |= (1 << PORTB5);			      // Set B5 as ON LED output
  //PORTB |= (1 << PORTB5);           // Turn ON LED on
  DDRD &= ~(1 << PORTD2);           // Set D2 as pulse button input
  PORTD |= (1 << PORTD2);            // Internal pull-up
  DDRD |= (1 << PORTD3);            // Set D3 as pulse output
  DDRB |= (1 << PORTB5);            // Set B5 as pulse LED output

  while(1){
    if (!(PIND & (1 << PIND2))){    // Check if D2 pulse button is pressed
      PORTD |= (1 << PORTD3);
      PORTB |= (1 << PORTB5);
      _delay_ms(5000);
      PORTD &= ~(1 << PORTD3);
      PORTB &= ~(1 << PORTB5);
    }
  }
}