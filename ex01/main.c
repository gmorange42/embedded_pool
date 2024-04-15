#include <avr/io.h>

int main()
{
	DDRB |= (1 << DDB0); // set the bit 0 of Data Direction Register B to 1 (OUTPUT)
	PORTB |=  (1 << PB0);   // set the bit 0 of PORT B to 1 (LED on)
	return 0;
}
