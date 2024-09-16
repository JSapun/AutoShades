/* This file attempts to test the battVcc module by using another power supply and the SevenSeg to display.
 * Will be using atmel studio.
 * DIO - PORTC0
 * CLK - PORTC1
 * VDD - 5V
 * GND - GND
 * 
 * Results show that configuration for battery guage will need to be done with new devices and new power sources.
 * My guess is even though the battery will drain, we can see the slight difference after calibration.
 *
*/


#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h> 
#include <util/delay.h>    


// Main Settings
#define	TM1637_DIO_PIN			PORTC0 //4 --- problem with ds3231 chip, changed to C0,C1 ????
#define	TM1637_CLK_PIN			PORTC1 //5
#define	TM1637_DELAY_US			(5)
#define	TM1637_BRIGHTNESS_MAX	(7)
#define	TM1637_POSITION_MAX		(4)

// TM1637 commands
#define	TM1637_CMD_SET_DATA		0x40
#define	TM1637_CMD_SET_ADDR		0xC0
#define	TM1637_CMD_SET_DSIPLAY	0x80

// TM1637 data settings (use bitwise OR to contruct complete command)
#define	TM1637_SET_DATA_WRITE		0x00 // write data to the display register
#define	TM1637_SET_DATA_READ		0x02 // read the key scan data
#define	TM1637_SET_DATA_A_ADDR		0x00 // automatic address increment
#define	TM1637_SET_DATA_F_ADDR		0x04 // fixed address
#define	TM1637_SET_DATA_M_NORM		0x00 // normal mode
#define	TM1637_SET_DATA_M_TEST		0x10 // test mode

// TM1637 display control command set (use bitwise OR to consruct complete command)
#define	TM1637_SET_DISPLAY_OFF		0x00 // off
#define	TM1637_SET_DISPLAY_ON		0x08 // on

void clock_delay() {_delay_us(TM1637_DELAY_US); }

#define	TM1637_DIO_HIGH()		(PORTC |= _BV(TM1637_DIO_PIN))
#define	TM1637_DIO_LOW()		(PORTC &= ~_BV(TM1637_DIO_PIN))

#define	TM1637_DIO_OUTPUT()		(DDRC |= _BV(TM1637_DIO_PIN))
#define	TM1637_DIO_INPUT()		(DDRC &= ~_BV(TM1637_DIO_PIN))

#define	TM1637_DIO_READ() 		(((PINC & _BV(TM1637_DIO_PIN)) > 0) ? 1 : 0)

#define	TM1637_CLK_HIGH()		(PORTC |= _BV(TM1637_CLK_PIN))
#define	TM1637_CLK_LOW()		(PORTC &= ~_BV(TM1637_CLK_PIN))

static void TM1637_send_config(const uint8_t enable, const uint8_t brightness);
static void TM1637_send_command(const uint8_t value);
static void TM1637_start(void);
static void TM1637_stop(void);
static uint8_t TM1637_write_byte(uint8_t value);

static uint8_t _config = TM1637_SET_DISPLAY_ON | TM1637_BRIGHTNESS_MAX;
static uint8_t _segments = 0xff;
PROGMEM const uint8_t _digit2segments[] =
{
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F  // 9
};

void I2C_Enable()
{
	DDRC  |= _BV(TM1637_DIO_PIN) | _BV(TM1637_CLK_PIN); //output pins
}

void I2C_Disable()
{
	DDRC  &= ~(_BV(TM1637_DIO_PIN) | _BV(TM1637_CLK_PIN)); //input pins
}

void TM1637_init(const uint8_t enable, const uint8_t brightness)
{
	TM1637_send_config(enable, brightness);
}

void TM1637_enable(const uint8_t value)
{

	TM1637_send_config(value, _config & TM1637_BRIGHTNESS_MAX);
}

void TM1637_set_brightness(const uint8_t value)
{

	TM1637_send_config(_config & TM1637_SET_DISPLAY_ON,
		value & TM1637_BRIGHTNESS_MAX);
}

void TM1637_display_segments(const uint8_t position, const uint8_t segments)
{

	TM1637_send_command(TM1637_CMD_SET_DATA | TM1637_SET_DATA_F_ADDR);
	TM1637_start();
	TM1637_write_byte(TM1637_CMD_SET_ADDR | (position & (TM1637_POSITION_MAX - 1)));
	TM1637_write_byte(segments);
	TM1637_stop();
}

void TM1637_display_digit(const uint8_t position, const uint8_t digit)
{
	uint8_t segments = (digit < 10 ? pgm_read_byte_near((uint8_t *)&_digit2segments + digit) : 0x00);

	if (position == 0x01) {
		segments = segments | (_segments & 0x80);
		_segments = segments;
	}

	TM1637_display_segments(position, segments);
}

void TM1637_display_colon(const uint8_t value)
{

	if (value) {
		_segments |= 0x80;
	} else {
		_segments &= ~0x80;
	}
	TM1637_display_segments(0x01, _segments);
}

void TM1637_clear(void)
{
	uint8_t i;

	for (i = 0; i < TM1637_POSITION_MAX; ++i) {
		TM1637_display_segments(i, 0x00);
	}
}

void TM1637_send_config(const uint8_t enable, const uint8_t brightness)
{

	_config = (enable ? TM1637_SET_DISPLAY_ON : TM1637_SET_DISPLAY_OFF) |
		(brightness > TM1637_BRIGHTNESS_MAX ? TM1637_BRIGHTNESS_MAX : brightness);

	TM1637_send_command(TM1637_CMD_SET_DSIPLAY | _config);
}

void TM1637_send_command(const uint8_t value)
{
	TM1637_start();
	TM1637_write_byte(value);
	TM1637_stop();
}

void TM1637_start(void)
{
	I2C_Enable();

	TM1637_DIO_HIGH();
	TM1637_CLK_HIGH();
	clock_delay();
	TM1637_DIO_LOW();
}

void TM1637_stop(void)
{

	TM1637_CLK_LOW();
	clock_delay();

	TM1637_DIO_LOW();
	clock_delay();

	TM1637_CLK_HIGH();
	clock_delay();

	TM1637_DIO_HIGH();

	I2C_Disable();
}

uint8_t TM1637_write_byte(uint8_t value)
{
	uint8_t ack=0;

	for (uint8_t i = 0; i < 8; ++i)
	{
		TM1637_CLK_LOW();
		clock_delay();

		if (value & 0x01)
		{
			TM1637_DIO_HIGH();
		}
		else
		{
			TM1637_DIO_LOW();
		}

		TM1637_CLK_HIGH();
		clock_delay();

                value >>= 1;
	}

	TM1637_CLK_LOW();
	TM1637_DIO_INPUT();
	TM1637_DIO_HIGH();
	clock_delay();

	ack = TM1637_DIO_READ();
	if (ack)
	{
		TM1637_DIO_OUTPUT();
		TM1637_DIO_LOW();
	}
	clock_delay();

	TM1637_CLK_HIGH();
	clock_delay();

	TM1637_CLK_LOW();
	clock_delay();

	TM1637_DIO_OUTPUT();

	return ack;
}

void batt_ADC_init(void);
uint16_t batt_readVccVoltage(uint32_t multimeter);
uint16_t get_avg_batt(void);

int main(void){
    DDRB |= (1 << PORTB5);
    PORTB |= (1 << PORTB5);
    TM1637_init(1, 1); // Enable, Brightness
    TM1637_display_colon(0);
    batt_ADC_init();
	
    while(1){
        uint16_t vcc = get_avg_batt();
        TM1637_display_digit(0, vcc/1000); // Position, Value
        TM1637_display_digit(1, (vcc%1000)/100);
        TM1637_display_digit(2, (vcc%100)/10);
        TM1637_display_digit(3, (vcc%10));
		_delay_ms(1000);			// Pause before next round			
	}
}


void batt_ADC_init(void){
    ADMUX |= 0b01001110;                                // Set to 1.1v reference
    ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2);   // Prescale by 128 (by default)
	ADCSRA |= (1 << ADEN);  						    // Enable ADC
}

uint16_t batt_readVccVoltage(uint32_t bandgap) {     // Returns vcc in mV
    ADCSRA |= (1 << ADSC); 				             // Start conversion
    while ((ADCSRA & (1 << ADSC)) != 0){}            // Wait for completion
    uint16_t value = ADC;                            // Read result
    uint16_t vcc = (bandgap * 1024) / value;         // Calculate vcc based on varying bandgap
    return vcc;

    /* To calculate bandgap, you need to first run this function with bandgap set to 1100. Then note the vcc output.
    Using regular scaling bandgap_new/bandgap_old = vcc_new/vcc_old, get bandgap_new and use it as the new input.*/
}

uint16_t get_avg_batt(void) {     // Returns vcc in mV
    uint32_t val = 0;
    for(uint8_t i = 0; i < 20; i++){
        val += batt_readVccVoltage(993); // Bandgap set to 993mV
        _delay_ms(5);
    }
    val /= 20;
    return (uint16_t) val;
}