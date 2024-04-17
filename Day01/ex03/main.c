#include <avr/io.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

int main(void)
{
	uint8_t ratio = 1;
	SET_BITS(DDRB, (1<<1)); // set DDRB1 to OUTPUT
	CLR_BITS(DDRD, (1<<2) | (1<<4)); // set DDRD1 and DDRD4 to INPUT
	
	// Set Comparison Value
	OCR1A = F_CPU / 256 / 10;

	// Set the Input Capture Unit 
	ICR1 = F_CPU / 256;

	// Set Timer top to mode 14
	SET_BITS(TCCR1A, (1<<WGM11));
	SET_BITS(TCCR1B, (1<<WGM12) | (1<<WGM13));

	// Set Clock parameter
	SET_BITS(TCCR1B, (1<<CS12)); 

	// Set Timer output
	SET_BITS(TCCR1A, (1<<COM1A1));

	while (1)
	{
		if ((!CHK_BITS(PIND, (1<<2))) && ratio < 10) // If SW1 is pressed and ratio < 10
		{
			OCR1A = (ICR1 / 10) * (++ratio); // Update of the Comparison Value
			_delay_ms(50);
			while (!CHK_BITS(PIND, (1<<2))) // While SW1 is pressed
			{
				_delay_ms(50); // Wait
			}
		}
		else if ((!CHK_BITS(PIND, (1<<4))) && ratio > 1)// If SW2 is pressed and ratio > 0
		{
			OCR1A = (ICR1 / 10) * (--ratio); // Update of the Comparison Value
			_delay_ms(50);
			while (!CHK_BITS(PIND, (1<<4))) // While SW1 is pressed
			{
				_delay_ms(50); // Wait
			}
		}
	}
	return (0);
}
