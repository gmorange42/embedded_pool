#include <avr/io.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define INDEX_COUNTER_MEM 0x30
#define FIRST_COUNTER_MEM 0x40

void print_binary(uint8_t nb)
{

	PORTB &= (0xFF << 3) & ~(1 << 4);              // 0b1111 1000 & 0x1110 1111
	PORTB |= (nb & 0b111) | ((nb & 0b1000) << 1); // 0b0000 0111 | 0b0001 0000
}

void debounce_SW1(void)
{
	for (uint32_t i = 0; i < F_CPU / 12 / 8; ++i)
	{}
	while (!(CHK_BITS(PIND, (1<<PD2))))
	{}
	for (uint32_t i = 0; i < F_CPU / 12 / 8; ++i)
	{}
}

void debounce_SW2(void)
{
	for (uint32_t i = 0; i < F_CPU / 12 / 8; ++i)
	{}
	while (!(CHK_BITS(PIND, (1<<PD4))))
	{}
	for (uint32_t i = 0; i < F_CPU / 12 / 8; ++i)
	{}
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
		index_counter %= 4;
		if (!(CHK_BITS(PIND, (1<<PD4))))
		{
			++index_counter;
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
