#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRB |= (1 << 0); // set the bit 0 in Data Direction Register B to 1 (OUTPUT)
	DDRD &= ~(1 << 2); // set the bit 2 in Data Direction Register D to 0 (OUTPUT)
	while (1)
	{
		if (PIND & (1 << 2)) // check if the bit 2 in Port Input Register is 1
		{
			PORTB &=  ~(1 << 0); // set PB0 to 0 (LED off)
		}
		else
		{
			PORTB |=  (1 << 0); // set PB0 to 1 (LED on)
		}
	}
	return 0;
}
