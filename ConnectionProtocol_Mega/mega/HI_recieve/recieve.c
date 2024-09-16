/*
 * Transmitter will be the mega to build the hardware interrupt.
 *
*/


#include <nrf24l01.h>
#include <avr/io.h>
#include <util/delay.h>


int main(void){ 				
  
  // Stage 1 --> receive correct message
  uint8_t received_payload = 0x00; //Actual message is 0xAA
  uint8_t actual_message = 0xAA;
  nrf24_device(RECEIVER, RESET);
  while(nrf24_receive(&received_payload, 1) == RECEIVE_FIFO_EMPTY); 
  if (received_payload == actual_message){
    blink3(4);
  }
  else {myprintf("Error, different message: %x\n\r", received_payload);}
  

  
  return 0;
}