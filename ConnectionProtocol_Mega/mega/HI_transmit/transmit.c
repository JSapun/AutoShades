
#include <nRF24L01.h>
#include <ioE28.h>
#include <USARTE28.h>

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
  USART_Init();   				// Initalize USUART COM for Screen
  myprintf("Starting\n\r");
  uint8_t known_message = 0XAA;                      
  uint8_t received_payload = 0x00; // should be returned as known_message

  // Stage 1 --> transmit known message
  nrf24_device(TRANSMITTER, RESET);
  for (uint8_t i = 0; i < 50; i++){
    while(nrf24_transmit(&known_message, 1, NO_ACK_MODE) == TRANSMIT_FAIL);
    while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
  }
  myprintf("Sent: %x\n\r", known_message);
  
  // Stage 2 --> recieve same message
  nrf24_device(RECEIVER, RESET);      
  while(nrf24_receive(&received_payload, 1) == RECEIVE_FIFO_EMPTY); 
  if (received_payload == known_message){
    myprintf("Connected!\n\r");
  }
  else {myprintf("Error, different message: %x\n\r", received_payload);}
  
  return 0;
}