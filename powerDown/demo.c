
#include <stdlib.h>
#include <avr/io.h>	
#include <avr/interrupt.h> 
#include <ioE28.h>
#include <USARTE28.h>


/* FUNCTIONS */
void timer1_init(uint16_t timeout);
void initPinChangeInterrupt(void);
/* GLOBAL VARIABLES */
volatile uint8_t timerFlag = 0; 
volatile uint8_t buttonPushed_D2;
volatile uint8_t buttonPushed_D3;
  /* CODE */
int main(void){

  DDRB |= (1 << DDD5);			      // configure bit 5 (onboard led) as output
  uint8_t ON_OFF_var = 0; 

  // Need to initalize inputs for two buttons 
  DDRD  &= ~(1 << DDD2);			    // D2 --> input (external interrupt)
  PORTD |= (1 << PORTD2);			    // Turn pullup on
  DDRD  &= ~(1 << DDD3);			    // D2 --> input (external interrupt)
  PORTD |= (1 << PORTD3);			    // Turn pullup on
  buttonPushed_D2 = 0;            // Set voltatile variable
  buttonPushed_D3 = 0;

  initPinChangeInterrupt();       // Start the interrupt system
  timer1_init(15625);             // timer start with 1 s
  USART_Init();                   // Start Serial comm

  while(1){
    if (timerFlag){
      timerFlag = 0;
    }
    if (buttonPushed_D2) {
      buttonPushed_D2 = 0; 
      PORTB |= (1 << PORTD5);
      myprintf("here\n\r");
    }
    else if (buttonPushed_D3) {
      buttonPushed_D3 = 0; 
      PORTB |= (1 << PORTD5);
      myprintf("here\n\r");
    }
    else {
      PORTB &= ~(1 << PORTD5);
    }

    if ((PIND & (1 << PIND5)) == 0) { // check if D5 is pressed by checking low
      myprintf("Down\n\r"); 
    }
    else if ((PIND & (1 << PIND6)) == 0) { // check if D6 is pressed by checking low
      myprintf("Up\n\r");
    }

    //if (ON_OFF_var){PORTB |= (1 << PORTD5);} else {PORTB &= ~(1 << PORTD5);} // Toggle LED every second
  }
}   

ISR(TIMER1_COMPA_vect) {
  timerFlag = 1;          						        
}

void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);						          // Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);						        // Enable timer interrupt
  OCR1A = timeout;						  		          // Load timeout value
  TCCR1B |= ((1 << CS10) | (1 << CS12));		  // Set prescaler to 1024
}

ISR(INT0_vect) {
  buttonPushed_D2 = 1; 				// Set flag to notify main
}

ISR(INT1_vect) {
  buttonPushed_D3 = 1; 				// Set flag to notify main
}

void initPinChangeInterrupt(void) {
	EICRA &= ~(1 << ISC01); // Set Low level behavior for INT0 (D2)
  EICRA &= ~(1 << ISC00); 

  EICRA &= ~(1 << ISC10); // Set low level behavior for INT1 (D3)
  EICRA &= ~(1 << ISC11);

	EIMSK |= (1 << INT0); // set External Interrupt Request to INT0 bit in EIMSK (D2)
  EIMSK |= (1 << INT1); // set External Interrupt Request to INT1 bit in EIMSK (D3)

  sei();							  // Reset Ports for Interrupt
}
