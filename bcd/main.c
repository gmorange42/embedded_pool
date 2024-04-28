#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

uint8_t decimal_to_bcd(uint8_t decimal)
{
	uint8_t bcd = decimal / 10;
	bcd = (bcd<<4);
	bcd += decimal % 10;
	return (bcd);
}

uint8_t bcd_to_decimal(uint8_t hex)
{
	uint8_t decimal = (hex>>4);
	decimal = (decimal<<4);
	hex = (hex<<4);
	hex = (hex>>4);
	decimal += hex;
	return (decimal);
}

int main(void)
{
	uart_init();
	uint8_t ret = 0;
	while (1)
	{
//		uart_printhex(decimal_to_bcd(99));
//		uart_printstr("\r\n");
		ret = bcd_to_decimal(40);
		uart_tx((ret/10)+'0');
		uart_tx((ret%10)+'0');
		uart_printstr("\r\n");
	}
	return (0);
}
