


#include <avr/io.h>



int main(){
    DDRD |= (1 << PORTD4);
    DDRD |= (1 << PORTD5);
    DDRD |= (1 << PORTD6);
    PORTD |= (1 << PORTD4);
    PORTD |= (1 << PORTD5);
    PORTD |= (1 << PORTD6);
    
    return 0;
}
