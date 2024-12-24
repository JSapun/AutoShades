

//#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h> 


void timer1_init(uint16_t timeout);
volatile uint8_t timer1Flag = 0; 


int main(){
    DDRB |= (1 << PORTB5);
    PORTB |= (1 << PORTB5);
    timer1_init(15625); // 1s
    while(1){
        if (timer1Flag){ // Every 1 second, only one thing happening per iteration
            timer1Flag = 0;
            PORTB ^= (1 << PORTB5);
        }
    }
    return 0;
}

ISR(TIMER1_COMPA_vect) {
    timer1Flag = 1;          						        // change flag
}

void timer1_init(uint16_t timeout) {
    TCCR1B |= (1 << WGM12);						    // Set mode to CTC
    TIMSK1 |= (1 << OCIE1A);						    // Enable timer interrupt
    OCR1A = timeout;						  		    // Load timeout value
    TCCR1B |= ((1 << CS10) | (1 << CS12));		    // Set prescaler to 1024
    sei();
}