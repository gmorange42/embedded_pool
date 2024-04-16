#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	uint8_t ratio = 1;
	DDRB |= (1<<1); // set DDRB1 to OUTPUT
	DDRD &= ~0b00010100; // set DDRD1 and DDRD4 to INPUT
	
	// Set Comparison Value
	OCR1A = F_CPU / 256 / 10;

	// Set the Input Capture Unit 
	ICR1 = F_CPU / 256;

	// Set Timer top to mode 14
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12);
	TCCR1B |= (1<<WGM13);

	// Set Clock parameter
	TCCR1B |= (1<<CS12); 

	// Set Timer output
	TCCR1A |= (1<<COM1A1);
	while (1)
	{
		if (!(PIND & (1<<2)) && ratio < 10)
		{
			OCR1A = (ICR1 / 10) * (++ratio);
			_delay_ms(50);
			while (!(PIND & (1<<2)))
			{
				_delay_ms(50);
			}
		}
		else if (!(PIND & (1<<4)) && ratio > 1)
		{
			OCR1A = (ICR1 / 10) * (--ratio);
			_delay_ms(50);
			while (!(PIND & (1<<4)))
			{
				_delay_ms(50);
			}
		}
	}
	return (0);
}
