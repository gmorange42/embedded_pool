#include <avr/io.h>

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
	EEAR = addr;  // Set address
	SET_BITS(EECR, (1<<EERE)); // Recover value
	return (EEDR);
}

bool check_offset(size_t offset, size_t length)
{
	if (offset < 0x05 ||
			(read_eeprom(offset - 1) == MAGIC_NUMBER &&
			 read_eeprom(offset - 2) == MAGIC_NUMBER) ||
			offset + length > 1023)
		return (false);
	for (uint8_t i = offset; i > 0; ++i)
	{
		if (read_eeprom(i) == MAGIC_NUMBER && read_eeprom(i + 1) == MAGIC_NUMBER)
		{
			if ((((uint16_t)(read_eeprom(i + 2)<<8) | (uint16_t)(read_eeprom(i + 3))) +
					(uint16_t)read_eeprom(i) + 4) > offset)
				return (false);
		}
	}
}

int main(void)
{
	uint8_t counter_value_tab[4];
	uint8_t index_counter;

	for (uint8_t i = 0; i < 4; ++i)
	{
		counter_value_tab[i] = read_eeprom(FIRST_COUNTER_MEM + i);
	}
	index_counter = read_eeprom(INDEX_COUNTER_MEM);
	SET_BITS(DDRB, (1<<PB0));
	SET_BITS(DDRB, (1<<PB1));
	SET_BITS(DDRB, (1<<PB2));
	SET_BITS(DDRB, (1<<PB4));

	CLR_BITS(DDRD, (1<<PD2));
	CLR_BITS(DDRD, (1<<PD4));

	CLR_BITS(EECR, (1<<EEPM0) | (1<<EEPM1)); // Set Programming Mode

	while (1)
	{
		uart_printhex(test);
		if (!(CHK_BITS(PIND, (1<<PD4))))
		{
			++index_counter;
			index_counter %= 4;
			write_eeprom(INDEX_COUNTER_MEM, index_counter);
			debounce_SW2();
		}
		counter_value_tab[index_counter] %= 16;
		print_binary(counter_value_tab[index_counter]);
		if (!(CHK_BITS(PIND, (1<<PD2))))
		{
			write_eeprom(FIRST_COUNTER_MEM + index_counter, ++counter_value_tab[index_counter]);
			print_binary(counter_value_tab[index_counter]);
			debounce_SW1();
		}
	}
	return (0);
}
