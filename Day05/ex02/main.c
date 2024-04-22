#include <avr/io.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define MAGIC_NUMBER 0X67
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

void write_eeprom(uint8_t addr, uint8_t value)
{
	while (CHK_BITS(EECR, (1<<EEPE))) // Wait Write Enable Bit becomes zero
	{}
	while (CHK_BITS(SPMCSR, (1<<SPMEN))) // Wait Store Program Memory Bit becomes zero
	{}
	EEAR = addr; // Set address
	EEDR = value; // Set value
	SET_BITS(EECR, (1<<EEMPE)); // Set Master Write Enable
	SET_BITS(EECR, (1<<EEPE)); // Set Write Enable
}

uint8_t read_eeprom(uint8_t addr)
{
	while (CHK_BITS(EECR, (1<<EEPE))) // Wait Write Enable Bit becomes zero
	{}
	while (CHK_BITS(SPMCSR, (1<<SPMEN))) // Wait Store Program Memory Bit becomes zero
	{}
	EEAR = addr; // Set address
	SET_BITS(EECR, (1<<EERE)); // Recover value
	return (EEDR);
}

void check_and_write(uint8_t offset, uint8_t byte)
{
	if (read_eeprom(offset) !=  byte)
	{
		write_eeprom(offset, byte);
	}
}

bool safe_eeprom_read(void* buffer, size_t offset, size_t length)
{
	if (offset < 0x02 || offset + length > 1023)
		return (false);
	if (read_eeprom(offset - 1) != MAGIC_NUMBER || read_eeprom(offset - 2) != MAGIC_NUMBER)
		return (false);
	for (size_t i = 0; i < length; ++i)
	{
//		uart_tx(read_eeprom(offset + i));
//		uart_tx(' ');
		*((uint8_t *)buffer + i) = read_eeprom(offset + i);
	}
	return (true);
}

bool safe_eeprom_write(void* buffer, size_t offset, size_t length)
{
	uart_printstr("Envie de crever ?");
	if (offset < 0x02 || offset + length > 1023)
		return (false);
	uart_printstr("ROGER");
	check_and_write(offset - 1, MAGIC_NUMBER);
	check_and_write(offset - 2, MAGIC_NUMBER);
	uart_printstr("BERNIE");
	unsigned char* byteBuffer = (unsigned char*) buffer;
	for (size_t i = offset; i < offset + length; ++i)
	{
		uart_printstr("TEST ");
		uart_tx(byteBuffer[i]);
		uart_tx(*((uint8_t *)buffer + i));
		uart_printstr("]");
//		check_and_write(i, *((uint8_t *)buffer + i));
		check_and_write(i, byteBuffer[i]);
	}
	return (true);
}


int main(void)
{
	uart_init();
	uint8_t lechat[5] = "chat";
	if (safe_eeprom_write(lechat, 0x04, 4))
		SET_BITS(PORTB, (1<<PB0));
	uint8_t test[5] = {0};
	if (safe_eeprom_read(test, 0x04, 4))
		SET_BITS(PORTB, (1<<PB1));
	while (1)
	{
		if (safe_eeprom_write(lechat, 0x04, 4))
			SET_BITS(PORTB, (1<<PB0));
		safe_eeprom_read(test, 0x04, 4);
		uart_printstr("\n\r");

		_delay_ms(500);
	}
	return (0);
}
