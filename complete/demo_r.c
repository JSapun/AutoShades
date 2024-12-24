// Reciever complete file, transciever, RTC, 7Seg, Motor control, Shade position algorithm, 1 LEd, 1 3-way switch
// Device does not go to sleep, switch case -- hardware interrupt on D2 (removed),
// 

/* Uncertainties:
*   - Motor rotation  
*   - From transciever tests, you will wait forever with function call. Need to call loop with timer 
*/

// 1.) Display time, 2.) Calibrate 3.) Handle alarm 4.) recieve potential message


#include <avr/io.h>
#include <avr/interrupt.h> 

#include <l293dMotor.h>
#include <i2c.h>
#include <ioE28.h>
#include <USARTE28.h>
#include <nrf24l01.h>
#include <ds3231RTC.h>
#include <tm1637SevenSeg.h>

 
void timer0_init(void);
void timer1_init(uint16_t timeout);
void display_clock(struct rtc_time data);
uint8_t connection_protocol(void);
void set_new_alarm(uint8_t alarm, struct rtc_time data);
volatile uint8_t buttonPushed_D2 = 0;
volatile uint16_t ticks = 0; 		
volatile uint8_t timer1Flag = 0; 

int main(){
    l293d_init();
    USART_Init();
    i2c_init();
    ds3231_init();
    TM1637_init(1, 1); // Enable, Brightness
    
    timer0_init();
    timer1_init(15625); // 1s
    sei();

    DDRD &= ~(1<<PORTD0); // Calibrate Switches
    PORTD |= (1<<PORTD0); 
    DDRD &= ~(1<<PORTD1);
    PORTD |= (1<<PORTD1); 
    DDRB |= (1<<PORTB2); // Calibrate LED
    PORTB &= ~(1 << PORTB2);

    //uint8_t top=0;
    uint16_t bot=5; // idk
    uint16_t cur=0;

    uint8_t payload;
    struct rtc_time out = {0};
    //struct rtc_time alarm = {0};

    myprintf("Start\n\r");

    while(1){
        if (timer1Flag){ // Every 1 second, only one thing happening per iteration
            timer1Flag = 0;
            myprintf("Looped\n\r");
            

            /* 1.) Read and display clock data */ 
            ds3231_read_time(&out);
            display_clock(out);  
            

            /* 2.) Read Switches for calibration, cannot move curtains */
            if (!(PIND & (1 << PIND0))){ 
                cur = 0; // Set to top, 0s
                PORTB |= (1 << PORTB2);
                while(timer1Flag==0){}; // Wait 1 sec
                timer1Flag=0;
                PORTB &= ~(1 << PORTB2);
            }
            else if (!(PIND & (1 << PIND1))){
                bot = cur;
                PORTB |= (1 << PORTB2);
                while(timer1Flag==0){}; // Wait 1 sec
                timer1Flag=0;
                PORTB &= ~(1 << PORTB2);
            }


            /* 3.) Handle clock alarms */
            else if ((DS3231_get_status_reg() & 0b00000011)!=0){ // Check register and position shade accordingly
                // Still need to change in recieving
                if (DS3231_get_status_reg() & 0b00000001){ // Alarm 1, go up
                    ticks = 0;
                    while(cur!=0){
                        if (ticks >= 12){ //0.1s
                            ticks = 0;
                            cur -= 1; // maybe increment of 1/10 sec
                        }
                        l293d_clockwise();
                        l293d_start();
                    }
                    ds3231_clear_alarm(1); 
                }
                else { // Alarm 2
                    ticks = 0; 
                    while(cur!=bot){
                        if (ticks >= 12){ //0.1s
                            ticks = 0;
                            cur += 1; // maybe increment of 1/10 sec
                        }
                        l293d_counterclockwise();
                        l293d_start();
                    }
                    ds3231_clear_alarm(2);
                }
            } 


            /* 4.) Recieve data */ 
            else {
                payload = connection_protocol();
                uint8_t up = (payload & 0b01000000);
                uint8_t down = (payload & 0b00100000);
                uint8_t alarm1 = (payload & 0b00010000);
                uint8_t alarm2 = (payload & 0b00001000);

                if (alarm1 || alarm2){ // Read both Clock Alarms
                    myprintf("alarm\n\r");
                    while (alarm1 || alarm2){
                        if (alarm1){ ds3231_read_alarm(1, &out); }
                        else { ds3231_read_alarm(2, &out); }
                        display_clock(out);
                        out.min+=10;                    // Need buffer for subtraction
                        if (up){ out.min += 5; }
                        else if (down){ out.min -= 5; }
                        if (alarm1){ set_new_alarm(1, out); }
                        else { set_new_alarm(2, out); }
                        

                        payload = connection_protocol();
                        up = ((payload & 0b01000000) != 0);
                        down = ((payload & 0b00100000) != 0);
                        alarm1 = ((payload & 0b00010000) != 0);
                        alarm2 = ((payload & 0b00001000) != 0);
                    }
                }
                else if (up || down){
                    ticks = 0;
                    while(up){ // No reason to check other inputs
                        myprintf("up\n\r");
                        l293d_clockwise();
                        l293d_start();  
                        if (ticks >= 12){ //0.1s
                            ticks = 0;
                            cur -= 1; // maybe increment of 1/10 sec
                        }
                        payload = connection_protocol();
                        up = ((payload & 0b01000000) != 0);
                    }
                    while(down){
                        myprintf("down\n\r");
                        l293d_counterclockwise();
                        l293d_start();
                        if (ticks >= 12){ //0.1s
                            ticks = 0;
                            cur += 1; // maybe increment of 1/10 sec
                        }
                        payload = connection_protocol();
                        down = ((payload & 0b00100000) != 0);
                    } 
                    l293d_stop();
                }
                else {
                    // No inputs
                    continue;
                }
            }  
        }                   
    }
    return 0;
}


ISR(TIMER0_OVF_vect) {
    ticks++;  // Increment ticks on overflow
}

ISR(TIMER1_COMPA_vect) {
    timer1Flag = 1;          						        // change flag
}

void timer0_init(void) {
    TCCR0A = 0;                 // Set Timer0 to Normal mode
    TCCR0B |= (1 << CS02);      // Set prescaler to 256
    TIMSK0 |= (1 << TOIE0);     // Enable overflow interrupt
}

void timer1_init(uint16_t timeout) {
    TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
    TCCR1B |= (1 << WGM12);						    // Set mode to CTC
    TIMSK1 |= (1 << OCIE1A);						    // Enable timer interrupt
    OCR1A = timeout;						  		    // Load timeout value
    TCCR1B |= ((1 << CS10) | (1 << CS12));		    // Set prescaler to 1024
}

void display_clock(struct rtc_time data){
    TM1637_display_digit(0, data.hour/10); // Position, Value
    TM1637_display_digit(1, data.hour%10);
    TM1637_display_digit(2, data.min/10);
    TM1637_display_digit(3, data.min%10);
    if (data.sec < 30){
        TM1637_display_colon(0);
    }
    else {
        TM1637_display_colon(1);
    }
}

uint8_t connection_protocol(void){
    uint8_t known_message = 0XAA;                      
    uint8_t received_payload = 0x00; // should be returned as known_message
    uint8_t secs = 0;

    // Stage 1 --> recieve same message
    nrf24_device(RECEIVER, RESET);  
    myprintf("Recieving...\n\r");        
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
        // Stage 2 --> transmit known message to complete connection protocol
        nrf24_device(TRANSMITTER, RESET);
        secs = 0;
        while (secs < 5){
            while(nrf24_transmit(&known_message, 1, NO_ACK_MODE) == TRANSMIT_FAIL); // Transmit for 5sec
            while(nrf24_transmit_status() == TRANSMIT_IN_PROGRESS);
            if (timer1Flag){
                timer1Flag = 0;
                secs++;
            }
        }
        return 1;
    }
    else { // Assumes connection protocol previously passed, so will do stuff
        if (received_payload != 0){ // Command enable bit
            myprintf("payload: %d\n\r",received_payload);
            return received_payload;
        }
        else{
            // Don't do anything, error, or nothing transmitting
            myprintf("Recieved 0\n\r");
            return 0;
        }
    }
}

void set_new_alarm(uint8_t alarm, struct rtc_time data){
    // Boundries
    if (data.min >= 70){ 
        data.hour += 1;
        data.min = 0; 
        if (data.hour >=24){ data.hour = 0; }
    } 
    else if (data.min < 10){ 
        if (data.hour == 0){
            data.hour = 23;
            data.min = 55;
        }
        else {
            data.hour -= 1;
            data.min = 55; 
        }
    }
    else {
        data.min -= 10;
    }

    ds3231_write_alarm(alarm, &data);
}


