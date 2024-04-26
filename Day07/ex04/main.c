#include "rgb.h"
#include <avr/io.h>
#include <util/delay.h>

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

	// Set Internal 1.1V Voltage Reference with external capacitor at AREF pin
	SET_BITS(ADMUX, (1<<REFS0));
}

void set_leds(uint16_t val)
{
	for (uint8_t i = 1; i < 4; ++i)
	{
		if (val > 255 * i)
			SET_BITS(PORTB, (1<<(i - 1)))
		else
			CLR_BITS(PORTB, (1<<(i - 1)));
	}
	if (val == 1023)
		SET_BITS(PORTB, (1<<PB4))
	else
		CLR_BITS(PORTB, (1<<PB4));
	wheel(val / 4);
}

int main(void)
{
	SET_BITS(DDRB, (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB4));
	SET_BITS(DDRD, (1<<PD3) | (1<<PD5) | (1<<PD6));
	adc_init();
	init_rgb();
	while (1)
	{
		//ADB Start Conversion
		SET_BITS(ADCSRA, (1<<ADSC));
	
		// While conversion in progress
		while (CHK_BITS(ADCSRA, (1<<ADSC)))
		{}
		set_leds(ADC);
		_delay_ms(20);
	}
	return (0);
}
