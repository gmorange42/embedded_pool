#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

int nack = 0;

void uart_printhex(uint8_t n);
void uart_printstr(uint8_t* str);

void i2c_init(void)
{
	TWBR = ((F_CPU / 100000) - 16) / 2; // Set the Value of the TWI Bit Rate Register
}


void i2c_start(uint8_t addr, uint8_t wr)
{
	//WR -> [Read = 1] [Write = 0]
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))) {} // Wait for TWINT Flag set. Indicate the START
						 // condition has been transmitted
//	uart_printhex(TWSR); // Print Status Code for Master Mode
	if (TW_STATUS != TW_START && TW_STATUS != 0x10) // Check value of TWI Status Register. Mask prescaler bits.
	{
		uart_printstr("TW_STATUS FAILED");
		return;
	}
	TWDR = (addr<<1) | wr; // Load Device Adrress into TWDR Register and Read/Write.
//	uart_printstr("ROGER");
//	uart_printhex(TWDR);
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the SLA_W has
					// been transmitted, and ACK/NACK has been received.
//	uart_printhex(TWSR); // Print Status Code for Master Mode
	if (TW_STATUS != TW_MT_SLA_ACK) // Check value of TWI Status Register. Mask prescaler bits.
					 // If status different from MT_SLA_ACK go to ERROR.
		return;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); // Send STOP condition
}

void i2c_write(unsigned char data)
{
	TWDR = data; // Load Device Adrress into TWDR Register and Read/Write.
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the SLA_W has
					// been transmitted, and ACK/NACK has been received.
//	uart_printhex(TWSR); // Print Status Code for Master Mode
	if (TW_STATUS != TW_MT_DATA_ACK) // Check value of TWI Status Register. Mask prescaler bits.
					 // If status different from MT_SLA_ACK go to ERROR.
		return;
}

void i2c_read(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (nack<<TWEA); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the SLA_W has
					// been transmitted, and ACK/NACK has been received.
//	uart_printhex(TWSR); // Print Status Code for Master Mode
	if (TW_STATUS != TW_MR_DATA_ACK && TW_STATUS != TW_MR_DATA_NACK) // Check value of TWI Status Register. Mask prescaler bits.
					 // If status different from MT_SLA_ACK go to ERROR.
		return;
	uart_printhex(TWDR); // Print Data
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

int main(void)
{
	uart_init();
	i2c_init();
	_delay_ms(100);
	i2c_start(0x38, 0);
	i2c_write(0x71);
	i2c_stop();
	i2c_start(0x38, 1);
	if (!CHK_BITS(TWDR, (1<<3)))
	{
		i2c_stop();
		i2c_start(0x38, 0);
		i2c_write(0xbe);
		i2c_write(0x08);
		i2c_write(0x00);
		_delay_ms(10);
	}
	i2c_stop();
	while (1)
	{
		_delay_ms(100);
		i2c_start(0x38, 0);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(80);
		i2c_start(0x38, 1);
		while (CHK_BITS(TWDR, (1<<7)))
			_delay_ms(80);
		nack = 1;
		for (uint8_t i = 0; i < 6; ++i)
		{
			i2c_read();
			uart_tx(' ');
		}
		nack = 0;
		i2c_read();
		uart_tx('\n');
		uart_tx('\r');
		i2c_stop();
	}
	return (0);
}
