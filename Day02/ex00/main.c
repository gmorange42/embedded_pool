#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

ISR(INT0_vect)
{
	SWI_BITS(PORTB, (1<<PB0));
}

int main(void)
{
	SET_BITS(DDRB, (1<<PB0)); // Set DDRB bit 0 to 1 (OUTPUT)

	SET_BITS(SREG, (1<<7)); // Set bit 7 in Status Register to 1 to enable Global Interrupt
	SET_BITS(EICRA, (1<<ISC01)); // Set the Interrupt Sence Controle to Falling
	SET_BITS(EIMSK, (1<<INT0)); // External Interrupt Request 0 Enable

	while (1)
	{
		_delay_ms(100);
	}

	return (0);
}
