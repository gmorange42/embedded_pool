#include "main.h"

int main()
{
	SET_BIT(DDRB, 0); // set the bit 0 in Data Direction Register B to 1 (OUTPUT)
	CLR_BIT(DDRD, 2); // set the bit 2 in Data Direction Register D to 0 (OUTPUT)
	while (1)
	{
		if (!CHK_BIT(PIND, 2)) // check if the bit 2 in Port Input Register is 1
		{
			SWI_BIT(PORTB, 0); // switch PB0 (LED on/off)
			_delay_ms(50); // waits to manage the bounce effects 
			while (!CHK_BIT(PIND, 2)) // while the SW1 is pressed
			{
				_delay_ms(50);
			}	
		}
	}
	return 0;
}
