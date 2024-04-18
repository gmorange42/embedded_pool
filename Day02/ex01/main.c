#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

volatile uint8_t ratio = 0; // Use to update OCR1A (represents ignition time as a percentage)
volatile uint8_t up = 1; // Use to know if ratio should be increased or decreased

void change_ratio(void)
{
	OCR1A = up ? ++ratio : --ratio; // Update OCR1A
	if (ratio == 0 || ratio == 100)
	{
		SWI_BITS(up, (1<<0)); // Switch up
	}
}

ISR(TIMER0_OVF_vect) // Interrupt if Timer0 overflow
{
	change_ratio();
}

int main(void)
{
	// Set DDRB1 to OUTPUT
	SET_BITS(DDRB, (1<<1));
	
	// Set Status Register to Global Interrupt Enable
	SET_BITS(SREG, (1<<7));

	//TIMER0
	// Set Comparison Value
	OCR0A = (F_CPU / 1024 / 200);

	// Set top to mode 7
	SET_BITS(TCCR0A, (1<<WGM00) | (1<<WGM01));
	SET_BITS(TCCR0B, (1<<WGM02));

	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR0B, (1<<CS02) | (1<<CS00)); 

	// Set Timer/Counter0 Overflow Interrupt Enable
	SET_BITS(TIMSK0, (1<<TOIE0));


	// TIMER1
	// Set Comparison Value
	OCR1A = ratio;

	// Set the Input Capture Unit (represent the top of Timer1 in Mode 14) 
	ICR1 = 100;

	// Set Compare Output Mode (OC1A = 0 if compare is true, OC1A = 1 if BOTTOM);
	SET_BITS(TCCR1A, (1<<COM1A1));

	// Set top to mode 14
	SET_BITS(TCCR1A, (1<<WGM11));
	SET_BITS(TCCR1B, (1<<WGM12) | (1<<WGM13));

	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR1B, (1<<CS12) | (1<<CS10)); 

	while (1)
	{
	}
	return (0);
}
