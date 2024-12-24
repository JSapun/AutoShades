
#include <nRF24L01.h>
//#include <ioE28.h>
//#include <USARTE28.h>


int main(void){
    //USART_Init();   				// Initalize USUART COM for Screen
    nrf24_device(TRANSMITTER, RESET);
    DDRD &= ~(1 << PORTD2);
    PORTD |= (1 << PORTD2);     

    //myprintf("Starting\n\r");

    while(1){
        if (!(PIND & (1 << PIND2))){
            //myprintf("Pressed\n\r");

            uint8_t known_message = 0XAA;                      
            for (uint8_t i = 0; i < 50; i++){
                while(nrf24_transmit(&known_message, 1, NO_ACK_MODE) == TRANSMIT_FAIL);
                while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
            }
        }   
    }
    return 0;
}