#include <avr/io.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define SS PB2
#define MOSI PB3
#define MISO PB4
#define SCK PB5

volatile uint8_t leds[3][3] = {0};

void adc_init(void)
{
	// ADC Enable
	SET_BITS(ADCSRA, (1<<ADEN));

	// ADC Presclaler to 128
	SET_BITS(ADCSRA, (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

	// Set AVcc with external capacitor at AREF pin and Left Adjust Result
	SET_BITS(ADMUX, (1<<REFS0) | (1<<ADLAR));
}

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB= (1<<MOSI)|(1<<SCK)|(1<<SS);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;

	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
		;
}

void start_frame(void)
{
	// Start Frame |00000000|00000000|00000000|00000000|
	for (uint8_t i = 0; i < 4; ++i)
		SPI_MasterTransmit(0);
}

void set_color(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue)
{
	SPI_MasterTransmit(brightness); // Max Brightness (0xE0 to 0xFF) |111|Brightness value|
	SPI_MasterTransmit(blue); // BLUE
	SPI_MasterTransmit(green); // GREEN
	SPI_MasterTransmit(red); // RED
}

void end_frame(void)
{
	// End Frame |11111111|11111111|11111111|11111111|
	for (uint8_t i = 0; i < 4; ++i)
		SPI_MasterTransmit(0xFF);

}

void set_led(uint8_t nled, uint8_t color)
{
	//ADB Start Conversion
	SET_BITS(ADCSRA, (1<<ADSC));

	// While conversion in progress
	while (CHK_BITS(ADCSRA, (1<<ADSC)))
	{}
	leds[nled][color] = ADCH; 
	start_frame();
	set_color(0xE1, leds[0][0], leds[0][1], leds[0][2]);
	set_color(0xE1, leds[1][0], leds[1][1], leds[1][2]);
	set_color(0xE1, leds[2][0], leds[2][1], leds[2][2]);
	end_frame();
}

int main(void)
{
	CLR_BITS(DDRD , (1<<PD2) | (1<<PD4));
	adc_init();
	SPI_MasterInit();
	uint8_t color = 0;
	uint8_t nled= 0;
	while (1)
	{
		if (!(CHK_BITS(PIND, (1<<PD2))))
		{
			++color;
			color %= 3;
			_delay_ms(50);
			set_led(nled, color);
			while (!(CHK_BITS(PIND, (1<<PD2))))
			{}
			_delay_ms(50);
		}
		else if (!(CHK_BITS(PIND, (1<<PD4))))
		{
			++nled;
			nled %= 3;
			_delay_ms(50);
			while (!(CHK_BITS(PIND, (1<<PD4))))
			{}
			_delay_ms(50);
		}
	}
	return (0);
}
