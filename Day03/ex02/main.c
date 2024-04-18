#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_init(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // Set Baud Rate Register High
	UBRR0L = (unsigned char)UBRR_VALUE; // Set Baud Rate Register Low 
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receiver and Transmitter Enable
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0))) // While Data Register is not empty
	{
	}
	UDR0 = c; // Fill the data register with c
}

void uart_printstr(uint8_t* str)
{
	uint8_t c = 0;
	while (str[c])
	{
		uart_tx(str[c++]);
	}
}

char uart_rx(void)
{	
	while (!(UCSR0A & (1<<RXC0))) // While Data Register is not empty
	{
	}
	return (UDR0); // Return Usart Data Register
}

int main(void)
{
	uart_init();

	while (1)
	{
		uart_tx(uart_rx());
	}
	return (0);
}
