#include <avr/io.h>
#include <util/twi.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_printstr(uint8_t* str);
void uart_printhex(uint8_t n);

void i2c_init(void)
{
//	STE_BITS(TWSR, (1<<TWPS0) | (1<<TWPS1); // Set prescaler
	TWBR = ((F_CPU / 100000) - 16) / 2; // Set the Value of the TWI Bit Rate Register
	TWAR = (0x38<<1);
}

void i2c_start(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))) {} // Wait for TWINT Flag set. Indicate the START
						 // condition has been transmitted
	uart_printhex(TWSR);
	if (TW_STATUS != TW_START) // Check value of TWI Status Register. Mask prescaler bits.
		return;
	TWDR = TWAR; // Load Device Adrress into TWDR Register
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the SLA_W has
					// been transmitted, and ACK/NACK has been received.
	uart_printhex(TWSR);
	if (TW_STATUS != TW_MT_SLA_ACK) // Check value of TWI Status Register. Mask prescaler bits.
					 // If status different from MT_SLA_ACK go to ERROR.
		return;
//	uart_printstr("OK MA GUEULE (END OF FUNCTION)");
//	TWDR = "TEST";//DATA; // Load DATA into TWDR Register.
//	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of data.
//	while (!(TWCR & (1<<TWINT))) {} // wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
//	if ((TWSR & 0xf8) != TW_MT_DATA_ACK) // Check value of TWI Status Register. Mask prescaler bits.
//		return;		  // if status different from MT_DATA_ACK go to ERROR
//	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); // Transmit STOP condition
}

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

void uart_printhex(uint8_t n)
{
    char c = n >> 4;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
    c = n & 0x0F;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
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
	i2c_init();

	i2c_start();
	while (1)
	{
	}
	return (0);
}
