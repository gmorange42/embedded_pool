#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

volatile uint8_t ratio = 0;
volatile uint8_t up = 1;

void change_ratio(void)
{
	if (up)
	{
		OCR1A = ++ratio;
	}
	else
	{
		OCR1A = --ratio;
	}
	if (ratio == 0 || ratio == 100)
	{
		SWI_BITS(up, (1<<0));;
	}
}

ISR(TIMER0_OVF_vect)
{
	change_ratio();
}

int main(void)
{
	uint8_t ratio = 1;
	SET_BITS(DDRB, (1<<1)); // set DDRB1 to OUTPUT
	SET_BITS(DDRB, (1<<0)); // set DDRB0 to OUTPUT
	
	// Set Comparison Value for Timer1
	OCR1A = ratio;

	// Set Comparison Value for Timer0
	OCR0A = (F_CPU / 1024 / 200);

	// Set the Input Capture Unit 
	ICR1 = 100;

	// Set Timer1 top to mode 14
	SET_BITS(TCCR1A, (1<<WGM11));
	SET_BITS(TCCR1B, (1<<WGM12) | (1<<WGM13));

	// Set Timer0 top to mode 7
	SET_BITS(TCCR0A, (1<<WGM00) | (1<<WGM01));
	SET_BITS(TCCR0B, (1<<WGM02));

	// Set Clock parameter Timer1
	SET_BITS(TCCR1B, (1<<CS12)); 

	// Set Clock parameter Timer0
	SET_BITS(TCCR0B, (1<<CS02) | (1<<CS00)); 

	// Set Timer1 output
	SET_BITS(TCCR1A, (1<<COM1A1));

	SET_BITS(TIMSK0, (1<<TOIE0));

	sei();

	while (1)
	{
	}
	return (0);
}

//Timer0 sert a changer le ratio du rapport cyclique.
//Timer1 allume et eteind la led suivant la rapport cyclique.
