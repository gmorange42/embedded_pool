#include <avr/io.h>

int main(void)
{
	DDRB |=(1<<1); // Set DDRB bit 1 to 1 (OUTPUT)
	while (1)
	{
		PORTB ^= (1<<1); // Switch the value of PORT B bit 1
		for (uint32_t i = 0; i < F_CPU / 12; ++i) // Loop on 16000000 iterations / 12 (arbitrary value)
		{
		}
	}
	return (0);
}
