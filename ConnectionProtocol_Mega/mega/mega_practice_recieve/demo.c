



#include <avr/io.h>
#include <ioE28.h>
#include <USARTE28.h>
#include <util/delay.h>

int main(void ){ 
  USART_Init();   
  DDRB |= (1 << PORTB7);
  PORTB |= (1 << PORTB7);

  myprintf("hi\n\r");
  PORTB &= ~(1 << PORTB7);
  myprintf("hi\n\r");

  while(1){
    myprintf("yoo\n\r");
    _delay_ms(1000);
  }

  return 0; 
}