

//#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h> 

void initPinChangeInterrupt(void);
volatile uint8_t buttonPushed_D2 = 0;

int main(){
    DDRD  &= ~(1 << DDD2); // On button input (hardware interrupt)
    PORTD |= (1 << PORTD2); 
    DDRB |= (1 << PORTB5);
    PORTB |= (1 << PORTB5);
    initPinChangeInterrupt();
    sei();

    while(1){
        if (buttonPushed_D2 == 1){
            buttonPushed_D2 = 0;
            PORTB &= ~(1 << PORTB5);
        }
    }
    return 0;
}

ISR(INT0_vect) {
	buttonPushed_D2 = 1;            // Set flag to notify main
}

void initPinChangeInterrupt(void) {
	EICRA |= (1 << ISC01);       // Set interrupt on falling edge
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);        // Enable INT0 interrupt
}