#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "bit_wise.h"

void adc_init(void)
{
	// ADC Enable
	SET_BITS(ADCSRA, (1<<ADEN));

	// ADC Presclaler to 128
	SET_BITS(ADCSRA, (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));

	// Set Internal 1.1V Voltage Reference with external capacitor at AREF pin
	SET_BITS(ADMUX, (1<<REFS0) | (1<<REFS1));
}

void start_n_print(uint8_t mask)
{
	// Set Analog Channel
	SET_BITS(ADMUX, mask);

	//ADB Start Conversion
	SET_BITS(ADCSRA, (1<<ADSC));

	// While conversion in progress
	while (CHK_BITS(ADCSRA, (1<<ADSC)))
	{}

	uart_print_number(ADC * 25 / 314);

	// Clear Analog Channel
	CLR_BITS(ADMUX, mask);
	_delay_ms(20);
}

int main(void)
{
	uart_init();
	adc_init();
	while (1)
	{
		start_n_print(0b00001000);
		uart_printstr("\n\r");
	}
	return (0);
}
