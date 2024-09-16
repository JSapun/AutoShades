/*
 * Reciever will be the mega, as we will primarily be building the transmitter commands from the uno atmega328p chip.
 *
*/


#include <nrf24l01.h>
#include <avr/io.h>
#include <util/delay.h>


static void blink3(uint8_t n){ 
  PORTB |= (1 << PORTB7);
  _delay_ms(n*100);
  PORTB &= ~(1 << PORTB7);
  _delay_ms(n*100);
  PORTB |= (1 << PORTB7);
  _delay_ms(n*100);
  PORTB &= ~(1 << PORTB7);
  _delay_ms(n*100);
  PORTB |= (1 << PORTB7);
  _delay_ms(n*100);
  PORTB &= ~(1 << PORTB7);
}

int main(void){ 				
  DDRB |= (1 << PORTB7);

  // Stage 1 --> receive correct message
  uint8_t received_payload = 0x00; //Actual message is 0xAA
  uint8_t actual_message = 0xAA;
  nrf24_device(RECEIVER, RESET);
  while(nrf24_receive(&received_payload, 1) == RECEIVE_FIFO_EMPTY); 
  if (received_payload == actual_message){
    blink3(4);
  }
  else {while(1){blink3(1);}}
  

  // Stage 2 --> transmit back message
  nrf24_device(TRANSMITTER, RESET);
  for (uint8_t i = 0; i < 50; i++){
    while(nrf24_transmit(&received_payload, 1, NO_ACK_MODE) == TRANSMIT_FAIL);
    while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
  }
  PORTB |= (1 << PORTB7);
  
  return 0;
}