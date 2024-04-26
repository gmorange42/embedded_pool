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

void set_color(uint8_t brightness, uint8_t blue, uint8_t green, uint8_t red)
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

int main(void)
{
	SPI_MasterInit();

	start_frame();

	// Led Frame 32 bits (D6)
	set_color(0xFF, 0, 0, 0xFF);
				 
	// Led Frame 32 bits (D7)
	set_color(0xE0, 0, 0, 0);
				  
	// Led Frame 32 bits (D8)
	set_color(0xE0, 0, 0, 0);

	// End Frame
	end_frame();

	while (1)
	{
	}
	return (0);
}
