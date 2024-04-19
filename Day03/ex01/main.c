#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_printstr(uint8_t*);

ISR(TIMER1_OVF_vect) // Interrupt if Timer1 overflow
{
	uart_printstr("Hello World!\n\r\0");
}

void uart_init(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // Set Baud Rate Register High
	UBRR0L = (unsigned char)UBRR_VALUE; // Set Baud Rate Register Low 
	UCSR0B = (1<<TXEN0); // Transmitter Enable
}

void uart_tx(uint8_t c)
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

void set_timer1(void)
{
	// Set Status Register to Global Interrupt Enable
	SET_BITS(SREG, (1<<7));

	
	// TIMER1

	// Set the Input Capture Unit (represent the top of Timer1 in Mode 14) 
	ICR1 = F_CPU / 1024 * 2;

	// Set top to mode 14
	SET_BITS(TCCR1A, (1<<WGM11));
	SET_BITS(TCCR1B, (1<<WGM12) | (1<<WGM13));

	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR1B, (1<<CS12) | (1<<CS10)); 
	
	// Set Timer/Counter1 Overflow Interrupt Enable
	SET_BITS(TIMSK1, (1<<TOIE1));
}

int main(void)
{
	uart_init();
	set_timer1();

	while (1) {}
	return (0);
}
