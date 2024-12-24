#include <avr/io.h>
#include <avr/interrupt.h>

void timer0_init(void);
volatile uint16_t ticks = 0;

int main() {
    DDRB |= (1 << PORTB5);  // Set PB5 as output
    PORTB |= (1 << PORTB5); // Initialize LED to ON
    timer0_init();          // Initialize Timer0
    sei();                  // Enable global interrupts

    while (1) {
        if (ticks >= 122) {     // 122 overflows for ~1 second with prescaler 256
            ticks = 0;         // Reset ticks
            PORTB ^= (1 << PORTB5);  // Toggle LED
        }
    }
    return 0;
}

ISR(TIMER0_OVF_vect) {
    ticks++;  // Increment ticks on overflow
}

void timer0_init(void) {
    TCCR0A = 0;                 // Set Timer0 to Normal mode
    TCCR0B |= (1 << CS02);      // Set prescaler to 256
    TIMSK0 |= (1 << TOIE0);     // Enable overflow interrupt
}
