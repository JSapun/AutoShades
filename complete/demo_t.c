// Transmitter complete file, transciever, power down, 3 buttons, 2 switches, 3 leds, battery guage, timer for delay (10s for further input)
// Device go to sleep if connection protocol fails or no input for 10 seconds, controlled by buttonPushed_D2 = 0.
// Note that Clock 1 and Up button take priority

// 1.) Check batt level, 2.) try to establish connection 3.) if connection failed, go to sleep, 4.) process until no further input


#include <avr/io.h>
#include <avr/interrupt.h> 
#include <avr/sleep.h>
#include <avr/power.h>

#include <ioE28.h>
#include <USARTE28.h>
#include <nrf24l01.h>
#include <battVCC.h>

void initPinChangeInterrupt(void);
void timer1_init(uint16_t timeout);
uint8_t connection_protocol(void);
uint8_t check_connection(void);
void bootup(void);
volatile uint8_t buttonPushed_D2 = 0;
volatile uint8_t timer1Flag = 0; 

int main(){
    USART_Init();
    boot_up();

    uint16_t vcc;
    uint8_t command;
    uint8_t timer10s;

	while(1){
		if (buttonPushed_D2 == 1){
            myprintf("D2\n\r");
			/* 1.) Wake up, reset components */
            boot_up();

            /* 2.) Get battery level and determine if red LED should be turned on */
            batt_ADC_init();
            vcc = get_avg_batt();
            if (vcc < 2800){ PORTD |= (1 << PORTD4); } // Low battery red LED, cannot be turned off
            myprintf("battery: %d\n\r",vcc);

            /* 3.) Attempt connection with reciever, if fails 5 times, go to sleep */
            if (check_connection()!=0) { // 1 is failed, 0 is good
                buttonPushed_D2 = 0;
                PORTD &= ~(1 << PORTD6);
                continue;
            }
            PORTD |= (1 << PORTD6); // Connection good, turn green LED on

            /* 4.) Transmit message based on button and switch inputs, go to sleep if no input present for 10 seconds */
            timer10s = 0;
            while (timer10s<10){              
                if (timer1Flag) { //increment timer1 if no input
                    timer1Flag = 0;
                    timer10s++;

                    // Double check connection every sec because why not, only change green led status
                    //if (check_connection()) {PORTD &= ~(1 << PORTD6);}
                    //else {PORTD |= (1 << PORTD6);}
                    myprintf("timer10: %d\n\r", timer10s); 
                }

                command = 0b10000000; // Command bits: Enable, Up down, Clock 1, Clock 2
                if (!(PINC & (1 << PINC0))){// Check if clock 1 switch pressed
                    myprintf("switch1\n\r");
                    command |= 0b00010000;
                    timer10s = 0;
                }
                else if (!(PINC & (1 << PINC1))){// Check if clock 2 switch pressed
                    myprintf("switch2\n\r");
                    command |= 0b00001000;
                    timer10s = 0;
                }
                if (!(PINB & (1 << PINB1))){ // Check if up button pressed
                    myprintf("button1\n\r");
                    command |= 0b01000000;
                    timer10s = 0;
                    PORTD |= (1 << PORTD5); // Orange LED for transmitting
                    // Run motor
                }
                else if (!(PINB & (1 << PINB2))){ // Check if down button pressed
                    myprintf("button2\n\r");
                    command |= 0b00100000;
                    timer10s = 0;
                    PORTD |= (1 << PORTD5);
                }
                else {
                    myprintf("none\n\r");
                    command &= ~(1 << 7); // If no input, wait 10s, then sleep, transmit 0
                    PORTD &= ~(1 << PORTD5);
                }
                
                // Transmit!
                myprintf("transmitted: %d\n\r", command);
                nrf24_device(TRANSMITTER, RESET);
                while(nrf24_transmit(&command, 1, NO_ACK_MODE) == TRANSMIT_FAIL);
                while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
                myprintf("success transmit\n\r");
            }
            buttonPushed_D2 = 0;
            PORTD &= ~(1 << PORTD6); // For testing!!
        }
		else{
            
			// disable ADC
			/*ADCSRA = 0;

			// turn off various modules
			power_all_disable();
			
			set_sleep_mode (SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			
			cli();
			
			// turn off brown-out enable in software
			MCUCR |= (1 << BODS) | (1 << BODSE);
			MCUCR |= (1 << BODS);

			sei();
			sleep_cpu();*/
            myprintf("else\n\r");
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
    EIMSK |= (1 << INT0);        // Enable INT0 interrupt (D2)
}

ISR(TIMER1_COMPA_vect) {
  timer1Flag = 1;          						        // change flag
}

void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);						          // Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);						        // Enable timer interrupt
  OCR1A = timeout;						  		          // Load timeout value
  TCCR1B |= ((1 << CS10) | (1 << CS12));		  // Set prescaler to 1024
}
 
uint8_t connection_protocol(void){
    uint8_t known_message = 0XAA;                      
    uint8_t received_payload = 0x00; // should be returned as known_message

    PORTD |= (1 << PORTD6); // Green LED on

    // Stage 1 --> transmit known message
    nrf24_device(TRANSMITTER, RESET);
    uint8_t secs = 0;
    while(secs < 5){
        while(nrf24_transmit(&known_message, 1, NO_ACK_MODE) == TRANSMIT_FAIL); // Transmit for 5sec
        while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
        if (timer1Flag){
            timer1Flag = 0;
            secs++;
        }
    }
    myprintf("Sent: %x\n\r", known_message);

    PORTD &= ~(1 << PORTD6); // Green LED off
    
    // Stage 2 --> recieve same message
    nrf24_device(RECEIVER, RESET);      
    secs = 0;
    while (secs < 5){
        if (nrf24_receive(&received_payload, 1) != RECEIVE_FIFO_EMPTY){ // Recieve for 5sec, until recieved
            break;
        }
        if (timer1Flag){
            timer1Flag = 0;
            secs++;
        }
    } 
    if (received_payload == known_message){
        myprintf("Connected!\n\r");
    }
    else {
        myprintf("Error, different message: %x\n\r", received_payload);
        return 1;
    }

    return 0;
}

uint8_t check_connection(void){
    uint8_t check = 0;
    for (uint8_t i = 0; i < 5; i++){ // Give 5 attempts to connect, flash green LED in connection_protocol function
        if (connection_protocol()!=0){
            check = 1;
        }
        else{
            break;
        }
    }

    if (check){ // If failed to connect, sleep
        return 1; 
    }
    return 0;
}

void boot_up(void){
    //sleep_disable();

    DDRD |= (1 << PORTD4); // Red LED
    DDRD |= (1 << PORTD5); // Orange LED
    DDRD |= (1 << PORTD6); // Green LED
    DDRB  &= ~(1 << PORTB1); // Up button input
    DDRB  &= ~(1 << PORTB2); // Down button input
    DDRC  &= ~(1 << PORTC0); // CLock switch 1 input
    DDRC  &= ~(1 << PORTC1); // Clock switch 2 input
    PORTB |= (1 << PORTB1);  // Button internal pull up resistors
    PORTB |= (1 << PORTB2);
    PORTC |= (1 << PORTC0);
    PORTC |= (1 << PORTC1);
    DDRD  &= ~(1 << DDD2); // On button input (hardware interrupt)
    PORTD |= (1 << PORTD2); 

    initPinChangeInterrupt();
    timer1_init(15625);  // timer start with 1 s
    sei();               // Reset Ports for Interrupt, done in timer1
}