#include <avr/io.h>

int main(void)
{
	DDRB |= (1<<1); // set DDRB1 to OUTPUT
	
	// Set Comparison Value
	OCR1A = F_CPU / 1024 / 2;

	// Set Timer top to mode 15
	TCCR1A |= (1<<WGM10);
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12);
	TCCR1B |= (1<<WGM13);

	// Set Clock parameter
	TCCR1B |= (1<<CS10); 
	TCCR1B |= (1<<CS12); 

	// Set Timer output
	TCCR1A |= (1<<COM1A0);
	while (1)
	{
	}
	return (0);
}
