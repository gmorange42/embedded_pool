#include <avr/io.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define BLUE PD3
#define RED PD5
#define GREEN PD6

void adc_init(void)
{
	// ADC Enable
	SET_BITS(ADCSRA, (1<<ADEN));

	// ADC Presclaler to 128
	SET_BITS(ADCSRA, (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

	// Set Internal 1.1V Voltage Reference with external capacitor at AREF pin
	SET_BITS(ADMUX, (1<<REFS0));
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void init_rgb(void)
{
	//PD3 BLUE  OC2B
	//PD5 RED   OC0B
	//PD6 GREEN OC0A
	
	//---TIMER0---
	
	// SET Compare Output Register
	OCR0A = 0;
	OCR0B = 0;

	// SET Compare Output Mode
	SET_BITS(TCCR0A, (1<<COM0A1) | (1<<COM0B1));

	// Set Waveform Generation Mode (Mode 3 [Fast PWM])
	SET_BITS(TCCR0A, (1<<WGM00) | (1<<WGM01));
		
	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR0B, (1<<CS02) | (1<<CS00)); 


	//--TIMER2--
	
	// SET Compare Output Register
	OCR2B = 0;

	// SET Compare Output Mode
	SET_BITS(TCCR2A, (1<<COM2B1));

	// Set Waveform Generation Mode (Mode 3 [Fast PWM])
	SET_BITS(TCCR2A, (1<<WGM20) | (1<<WGM21));
		
	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR2B, (1<<CS22) |  (1<<CS21) | (1<<CS20)); 
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
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
