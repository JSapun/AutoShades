/*transmitter code example, transmits an ascending number every TIME_GAP milliseconds in NO_ACK mode*/
/*static payload length of 1 byte, 1Mbps datarate, -6 dbm rf transmit power, channel 32 of 125 chanels*/
#include <nrf24l01.h>
#include <ioE28.h>
#include <USARTE28.h>
 
#define TIME_GAP    500

uint8_t ascending_number = 0X00;                        


int main(void){
  USART_Init();   				// Initalize USUART COM for Screen
  nrf24_device(TRANSMITTER, RESET);     //initializing nrf24l01+ as a transmitter using one simple function
  myprintf("Start!\n\r");

  while(1){
    _delay_ms(TIME_GAP);


    while(nrf24_transmit(&ascending_number, 1, NO_ACK_MODE) == TRANSMIT_FAIL){      //wait until payload is loaded into TX buffer
      myprintf("Transmit fail!\n\r");
    }
    
    while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);      //poll the transmit status, make sure the payload is sent

    myprintf("Sent: %x\n\r", ascending_number);      //payload is sent without acknowledge successfully
    ascending_number++;
  }
} 