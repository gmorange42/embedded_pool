#include <avr/io.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define BLUE PD3
#define RED PD5
#define GREEN PD6

void control_color(uint8_t mask)
{
	SET_BITS(PORTD, mask);
	_delay_ms(1000);
	CLR_BITS(PORTD, mask);
}

int main(void)
{
	SET_BITS(DDRD, 0b01101000);
	while (1)
	{
		control_color((1<<RED));
		control_color((1<<GREEN));
		control_color((1<<BLUE));
	}
	return (0);
}
