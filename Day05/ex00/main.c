#include <avr/io.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

void print_binary(uint8_t nb)
{

	PORTB &= (0xFF << 3) & ~(1 << 4);              // 0b1111 1000 & 0x1110 1111
	PORTB |= (nb & 0b111) | ((nb & 0b1000) << 1); // 0b0000 0111 | 0b0001 0000
}

void debounce_SW1(void)
{
	for (uint32_t i = 0; i < F_CPU / 12 / 4; ++i)
	{}
	while (!(CHK_BITS(PIND, (1<<PD2))))
	{}
	for (uint32_t i = 0; i < F_CPU / 12 / 4; ++i)
	{}
}

void write_eeprom(uint8_t addr, uint8_t value)
{
	while (CHK_BITS(EECR, (1<<EEPE)))
	{}
	while (CHK_BITS(SPMCSR, (1<<SPMEN)))
	{}
	EEAR = addr; 
	EEDR = value;
	SET_BITS(EECR, (1<<EEMPE));
	SET_BITS(EECR, (1<<EEPE));
}

uint8_t read_eeprom(uint8_t addr)
{
	while (CHK_BITS(EECR, (1<<EEPE)))
	{}
	while (CHK_BITS(SPMCSR, (1<<SPMEN)))
	{}
	EEAR = addr; 
	SET_BITS(EECR, (1<<EERE));
	return (EEDR);
}

int main(void)
{
	uint8_t counter_value = read_eeprom(0x40);
	SET_BITS(DDRB, (1<<PB0));
	SET_BITS(DDRB, (1<<PB1));
	SET_BITS(DDRB, (1<<PB2));
	SET_BITS(DDRB, (1<<PB4));

	CLR_BITS(DDRD, (1<<PD2));

	CLR_BITS(EECR, (1<<EEPM0) | (1<<EEPM1)); // Set Programming Mode

	while (1)
	{
		counter_value %= 16;
		print_binary(counter_value);
		if (!(CHK_BITS(PIND, (1<<PD2))))
		{
			write_eeprom(0x40, ++counter_value);
			print_binary(counter_value);
			debounce_SW1();
		}
	}
	return (0);
}
