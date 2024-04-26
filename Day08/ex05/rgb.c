#include "rgb.h"

void init_rgb(void)
{
	//PD3 BLUE  OC2B
	//PD5 RED   OC0B
	//PD6 GREEN OC0A
	
	//---TIMER0---
	
	// SET Compare Output Register
	OCR0A = 0;
	OCR0B = 0;

	// SET Compare Output Mode
	SET_BITS(TCCR0A, (1<<COM0A1) | (1<<COM0B1));

	// Set Waveform Generation Mode (Mode 3 [Fast PWM])
	SET_BITS(TCCR0A, (1<<WGM00) | (1<<WGM01));
		
	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR0B, (1<<CS02) | (1<<CS00)); 


	//--TIMER2--
	
	// SET Compare Output Register
	OCR2B = 0;

	// SET Compare Output Mode
	SET_BITS(TCCR2A, (1<<COM2B1));

	// Set Waveform Generation Mode (Mode 3 [Fast PWM])
	SET_BITS(TCCR2A, (1<<WGM20) | (1<<WGM21));
		
	// Set Clock Source to clki/o/1024 (From prescaler)
	SET_BITS(TCCR2B, (1<<CS22) |  (1<<CS21) | (1<<CS20)); 
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}
