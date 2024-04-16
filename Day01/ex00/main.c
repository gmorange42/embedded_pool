#include <avr/io.h>

int main(void)
{
	DDRB |=(1<<1);
	while (1)
	{
		PORTB ^= (1<<1);
		for (uint32_t i = 0; i < F_CPU / 12; ++i)
		{
			;
		}
	}
	return (0);
}
