#include <avr/io.h>
#include <util/delay.h>

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

void uart_printhex(uint8_t n)
{
    char c = n >> 4;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
    c = n & 0x0F;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
}

void adc_init(void)
{
	// Set ADC Left Adjus Result
	SET_BITS(ADMUX, (1<<ADLAR)); 

	// ADC Enable
	SET_BITS(ADCSRA, (1<<ADEN));

	// ADC Presclaler to 128
	SET_BITS(ADCSRA, (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

	// Set AVcc with external capacitor at AREF pin
	SET_BITS(ADMUX, (1<<REFS0));

}

void start_n_print(uint8_t mask)
{
	// Set Analog Channel
	SET_BITS(ADMUX, mask);

	//ADB Start Conversion
	SET_BITS(ADCSRA, (1<<ADSC));

	// While conversion in progress
	while (CHK_BITS(ADCSRA, (1<<ADSC)))
	{}

	uart_printhex(ADCH);

	// Clear Analog Channel
	CLR_BITS(ADMUX, mask);
	_delay_ms(20);
}

int main(void)
{
	uart_init();
	adc_init();
	while (1)
	{
		start_n_print(0b00000000);
		uart_printstr(", ");
		start_n_print(0b00000001);
		uart_printstr(", ");
		start_n_print(0b00000010);
		uart_printstr("\n\r");
	}
	return (0);
}
