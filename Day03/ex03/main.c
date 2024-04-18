#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask)
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_init(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // Set Usart Baud Rate Register High
	UBRR0L = (unsigned char)UBRR_VALUE; // Set Usart Baud Rate Register Low 
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); // Receiver, Transmitter and Receiver Interrupt Enable
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0))) // While Data Register is not empty
	{
	}
	UDR0 = c; // Fill the Data Register with c
}

ISR(USART_RX_vect)
{
	uint8_t c = UDR0;
	if ((c >= 'a' && c <= 'z') || c >= 'A' && c <= 'Z') // If the char is a letter
	{
		uart_tx(SWI_BITS(c, (1<<5))); // Switch the case
	}
	else
	{
		uart_tx(c);
	}
}

int main(void)
{
	SET_BITS(SREG, (1<<7)); // Global Interrupt enable
	uart_init();

	while (1)
	{
	}
	return (0);
}
