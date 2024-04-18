#include <avr/io.h>
#include <util/delay.h>

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_init(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // Set Baud Rate Register High
	UBRR0L = (unsigned char)UBRR_VALUE; // Set Baud Rate Register Low 
	UCSR0B = (1<<TXEN0); // Transmitter Enable
}

void uart_tx(void)
{
	while (!(UCSR0A & (1<<UDRE0))) // While Data Register is not empty
	{
	}
	UDR0 = 'z'; // Fill the data register with 'z'
}

int main(void)
{
	uart_init();

	while (1)
	{
		uart_tx();
		_delay_ms(1000); // 1 Hz
	}
	return (0);
}
