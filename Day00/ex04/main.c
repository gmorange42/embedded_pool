#include "main.h"

int main()
{
	SET_BIT(DDRB, D1); // set the bit 0 in Data Direction Register B to 1 (OUTPUT)
	SET_BIT(DDRB, D2); // set the bit 1 in Data Direction Register B to 1 (OUTPUT)
	SET_BIT(DDRB, D3); // set the bit 2 in Data Direction Register B to 1 (OUTPUT)
	SET_BIT(DDRB, D4); // set the bit 4 in Data Direction Register B to 1 (OUTPUT)
	CLR_BIT(DDRD, SW1); // set the bit 2 in Data Direction Register D to 0 (OUTPUT)
	CLR_BIT(DDRD, SW2); // set the bit 4 in Data Direction Register D to 0 (OUTPUT)
	while (1)
	{
		if (!CHK_BIT(PIND, SW1)) // check if the bit 2 in Port Input Register is 0
		{
			increment();
		}
		if (!CHK_BIT(PIND, SW2)) // check if the bit 4 in Port Input Register is 0
		{
			decrement();
		}
	}
	return 0;
}

void increment(void)
{
	uint8_t n = 0;
	while (CHK_BIT(PINB, n)) // while the n bit == 1
	{
		SWI_BIT(PORTB, n); // swith n bit (1 to 0)
		++n;
	}
	if (n == 3) // jump since PB3 to PB4
	{
		++n;
	}
	SWI_BIT(PORTB, n); // switch the n bit (0 to 1)
	_delay_ms(50); // wait to manage the bounce effects 
	while (!CHK_BIT(PIND, SW1)) // while the SW1 is pressed (PIND 2 == 0)
	{
		_delay_ms(50);
	}	
}

void decrement(void)
{
	uint8_t n = 0;
	while (!CHK_BIT(PINB, n)) // while the n bit == 0
	{
		SWI_BIT(PORTB, n); // swith n bit (0 to 1)
		++n;
	}
	if (n == 3) // jump since PB3 to PB4
	{
		++n;
	}
	SWI_BIT(PORTB, n);
	_delay_ms(50); // wait to manage the bounce effects 
	while (!CHK_BIT(PIND, SW2)) // while the SW2 is pressed (PIND 4 == 0)
	{
		_delay_ms(50);
	}	
}
