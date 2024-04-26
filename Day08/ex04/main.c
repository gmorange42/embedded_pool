#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

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

void cycle(uint32_t D6, uint32_t D7, uint32_t D8)
{
	start_frame();
	set_color((D6>>24), (D6>>16), (D6>>8), D6);
	set_color((D7>>24), (D7>>16), (D7>>8), D7);
	set_color((D8>>24), (D8>>16), (D8>>8), D8);
	end_frame();
	_delay_ms(250);
}

void manage_one_led(uint32_t colors, uint8_t led)
{

	start_frame();
	for (uint8_t i = 6; i < 9; ++i)
	{
		if (i == led)
			set_color(0xFF, (colors>>16), (colors>>8), colors);
		else
			set_color(0xE0,0,0,0);

	}
	end_frame();
}

void check_one_led_cmd(uint8_t data[7])
{
	uint32_t hex = 0;
	if (data[0] != '#')
	{
		uart_printstr("ERROR, HASHTAG MISSING\n\r");
		return ;
	}
	if ((data[7] != 'D') || (data[8] != '6' && data[8] != '7' && data[8] != '8'))
	{
		uart_printstr("ERROR, INVALID LED\n\r");
		return ;
	}
	for (uint8_t i = 1; i < 7; ++i)
	{
		if (data[i] >= 'A' && data[i] <='F')
		{
			data[i] -= 'A';
			data[i] += 10;
		}
		else if (data[i] >= '0' && data[i] <= '9')
			data[i] -= '0';
		else
		{
			uart_printstr("ERROR, INVALID HEX\n\r");
			return ;
		}
		hex = (hex<<4);
		hex += data[i];
	}
	manage_one_led(hex, data[8] - '0');
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_color(0xFF, 255 - pos * 3, 0, pos * 3);
		set_color(0xFF, 0, pos * 3, 255 - pos * 3);
		set_color(0xFF, pos * 3, 255 - pos * 3, 0);
	} else if (pos < 170) {
		pos = pos - 85;
		set_color(0xFF, 0, pos * 3, 255 - pos * 3);
		set_color(0xFF, pos * 3, 255 - pos * 3, 0);
		set_color(0xFF, 255 - pos * 3, 0, pos * 3);
	} else {
		pos = pos - 170;
		set_color(0xFF, pos * 3, 255 - pos * 3, 0);
		set_color(0xFF, 255 - pos * 3, 0, pos * 3);
		set_color(0xFF, 0, pos * 3, 255 - pos * 3);
	}
}

uint8_t rainbow(uint8_t data[13])
{
	uint8_t full_rainbow[13] = "#FULLRAINBOW";

	for (uint8_t i = 0; i < 12; ++i)
		if (data[i] != full_rainbow[i])
		{
			uart_printstr("ERROR, NOT #FULLRAINBOW");
			return (0);
		}
	return (1);
}

volatile uint8_t rainbow_color = 0;
volatile uint8_t in_wheel = 0;

char modif_uart_rx(void)
{	
	while (!(UCSR0A & (1<<RXC0))) // While Data Register is not empty
	{
		if (in_wheel)
		{
			rainbow_color %= 255;
			start_frame();
			wheel(rainbow_color++);
			end_frame();
			_delay_ms(10);
		}
	}
	return (UDR0); // Return Usart Data Register
}

int main(void)
{
	uart_init();
	SPI_MasterInit();
	uint8_t input[13] = {0};
	uint8_t rx;
	uint8_t i = 0;
	while (1)
	{
		rx = modif_uart_rx();
		uart_tx(rx);
		input[i++] = rx;
		input[i] = 0;
		if (i > 13)
		{
			i = 0;
			uart_printstr("ERROR, COMMAND TO LONG\n\r");
		}
		else if (rx == '\r')
		{
			uart_tx(i);
			if (i != 10 && i != 13)
				uart_printstr("ERROR, CMD MISSING OR INCOMPLETE\n\r");
			else if (i == 10)
			{
				check_one_led_cmd(input);
				in_wheel = 0;
			}
			else
			{
				if (rainbow(input))
					in_wheel = 1;
			}
			i = 0;
			uart_printstr("\n\r");
		}
	}
	return (0);
}
