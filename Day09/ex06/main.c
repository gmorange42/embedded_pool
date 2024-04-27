#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

void adc_init(void)
{
	// ADC Enable
	SET_BITS(ADCSRA, (1<<ADEN));

	// ADC Presclaler to 128
	SET_BITS(ADCSRA, (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

	// Set AVcc with external capacitor at AREF pin
	SET_BITS(ADMUX, (1<<REFS0));
}

void init_seven_segments(void)
{
	i2c_init();
	i2c_start(0b00100000, 0); // Send address in write mode
	i2c_write(6); // Set command Configuration port 0 first
	i2c_write(0b00001111); // Set pins 7 6 5 3 in register 0 to output
	i2c_write(0b00000000); // Set all pins in register 1 to output 
	i2c_stop();
}

void print_digit(uint8_t digit, uint8_t place, uint8_t dot)
{
	uint8_t leds[11];
	leds[0] = 0b00111111;
	leds[1] = 0b00000110;
	leds[2] = 0b01011011;
	leds[3] = 0b01001111;
	leds[4] = 0b01100110;
	leds[5] = 0b01101101;
	leds[6] = 0b01111101;
	leds[7] = 0b00000111;
	leds[8] = 0b01111111;
	leds[9] = 0b01101111;
	leds[10] = 0b00000000;

	i2c_start(0b00100000, 0);  // send address in write mode
	i2c_write(2); //Send command Output port 0 first
	i2c_write(~(1<<(8 - place))); // Bits of register 0 LOW
	i2c_write((dot ? leds[digit]|=(1<<7) : leds[digit])); // Bits of register 1 equal to leds variable 
	i2c_stop();
}

void print_number(uint32_t number)
{
	print_digit(number / 1000, 4, 0);
	print_digit(10, 4, 0);

	print_digit(number / 100 % 10, 3, 0);
	print_digit(10, 3, 0);

	print_digit(number / 10 % 10, 2, 0);
	print_digit(10, 2, 0);

	print_digit(number % 10, 1, 0);
	print_digit(10, 1, 0);
}

int main(void)
{
	init_seven_segments();
	adc_init();
	while (1)
	{
		//ADB Start Conversion
		SET_BITS(ADCSRA, (1<<ADSC));

		// While conversion in progress
		while (CHK_BITS(ADCSRA, (1<<ADSC)))
		{}
		print_number(ADC);
	}
	return (0);
}
