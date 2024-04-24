#include <avr/io.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

#define BLUE PD3
#define RED PD5
#define GREEN PD6

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_init(void)
{
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // Set Baud Rate Register High
	UBRR0L = (unsigned char)UBRR_VALUE; // Set Baud Rate Register Low 
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receiver and Transmitter Enable
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0))) // While Data Register is not empty
	{
	}
	UDR0 = c; // Fill the data register with c
}

void uart_printstr(uint8_t* str)
{
	uint8_t c = 0;
	while (str[c])
	{
		uart_tx(str[c++]);
	}
}

char uart_rx(void)
{	
	while (!(UCSR0A & (1<<RXC0))) // While Data Register is not empty
	{
	}
	return (UDR0); // Return Usart Data Register
}

void uart_printhex(uint8_t n)
{
    char c = n >> 4;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
    c = n & 0x0F;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
}

void set_ddr(uint8_t led, uint8_t value)
{
	if (value)
		SET_BITS(DDRD, (1<<led))
	else
		CLR_BITS(DDRD, (1<<led));
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	set_ddr(RED, r);
	set_ddr(GREEN, g);
	set_ddr(BLUE, b);
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

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

uint8_t rgb[6] = {0};

void update_rgb(uint8_t c)
{
	if (c == '\r')
	{
		set_rgb((((rgb[0]<<4) | rgb[1])),
				(((rgb[2]<<4) | rgb[3])),
				(((rgb[4]<<4) | rgb[5])));
	}
	else
	{
		if (c >= 'a' && c <='f')
		{
			c -= 'a';
			c += 10;
		}
		else if (c >= '0' && c <= '9')
			c -= '0';
		else
			return ;

		for (uint8_t i = 0; i < 5; ++i)
			rgb[i] = rgb[i + 1];
		rgb[5] = c;
	}
}

int main(void)
{
	init_rgb();
	uart_init();
	uint8_t color = 0;
	uint8_t value;
	while (1)
	{
		value = uart_rx();
		uart_tx(value);
		update_rgb(value);
	}
	return (0);
}
