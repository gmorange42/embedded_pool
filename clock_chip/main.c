#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"
#include <avr/interrupt.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

void init_seven_segments(void)
{
	i2c_start(0b01000000); // Send address in write mode
	i2c_write(6); // Set command Configuration port 0 first
	i2c_write(0b00001111); // Set pins 7 6 5 3 in register 0 to output
	i2c_write(0b00000000); // Set all pins in register 1 to output 
	i2c_stop();
}

void print_digit(uint8_t digit, uint8_t place, uint8_t dot)
{
	uint8_t leds[11];
	leds[0] = 0b00111111;
	leds[1] = 0b00000110;
	leds[2] = 0b01011011;
	leds[3] = 0b01001111;
	leds[4] = 0b01100110;
	leds[5] = 0b01101101;
	leds[6] = 0b01111101;
	leds[7] = 0b00000111;
	leds[8] = 0b01111111;
	leds[9] = 0b01101111;
	leds[10] = 0b00000000;

	i2c_start(0b01000000); // Send address in write mode
	i2c_write(2); //Send command Output port 0 first
	i2c_write(~(1<<(8 - place))); // Bits of register 0 LOW
	i2c_write((dot ? leds[digit]|=(1<<7) : leds[digit])); // Bits of register 1 equal to leds variable 
	i2c_stop();
}

void print_number(uint32_t number, uint8_t dots_mask)
{
	print_digit(number / 1000, 4, CHK_BITS(dots_mask, (1<<3)));
	print_digit(10, 4, 0);

	print_digit(number / 100 % 10, 3, CHK_BITS(dots_mask, (1<<2)));
	print_digit(10, 3, 0);

	print_digit(number / 10 % 10, 2, CHK_BITS(dots_mask, (1<<1)));
	print_digit(10, 2, 0);

	print_digit(number % 10, 1, CHK_BITS(dots_mask, (1<<0)));
	print_digit(10, 1, 0);
}

// SET PARAMETRES IN HEX (year just 2 last digits)
void set_RTC(uint8_t second, uint8_t minutes, uint8_t hours, uint8_t days, uint8_t months, uint8_t year)
{
	i2c_start(0xA2); // i2c slave address, write
	i2c_write(0x00); // word address 0, next bytes are data
	i2c_write(0x00); // control/status 1, no test modes or POR override
	i2c_write(0x00); // control/status 2, no alarm/timer flags or interrupts
	i2c_write(0x00); // set seconds, clear voltage low detector
	i2c_write(minutes); // set minutes to 45
	i2c_write(hours); // set hours to 14
	i2c_write(days); // set days to 16
	i2c_write(0x05); // set weekdays to Friday, Monday is day 1
	i2c_write(months); // set month to 7 and century bit to 1
	i2c_write(year); // set years to 08
	i2c_write(0x80); // disable minute alarm and reset to 00
	i2c_write(0x80); // disable hour alarm and reset to 00
	i2c_write(0x80); // disable day alarm and reset to 00
	i2c_write(0x80); // disable weekday alarm and reset to 00
	i2c_write(0x80); // set frequency out to 32768 Hz e.g. for tuning
	i2c_write(0x00); // timer switched off
	i2c_stop();
}

uint16_t bcd_to_decimal(uint16_t hex)
{
	uint16_t decimal = 0;

	decimal = (hex & 0b1111000000000000)>>12;
	decimal *= 10;
	decimal += (hex & 0b0000111100000000)>>8;
	decimal *= 10;
	decimal += (hex & 0b0000000011110000)>>4;
	decimal *= 10;
	decimal += hex & 0b0000000000001111;
	return (decimal);
}

uint8_t get_minute(void)
{
	i2c_start(0xA2); // Send a START condition and the slave address for write (A2h).
	i2c_write(0x03); // Set the address pointer to 3 (Minutes) by sending 03h.
	i2c_stop();
	i2c_start(0xA3); // Send a RESTART condition or STOP followed by START and end the slave address for read (A3h).
	return (i2c_read(0) & 0b01111111); // Read Minutes.
}

uint8_t get_hour(void)
{
	i2c_start(0xA2); // Send a START condition and the slave address for write (A2h).
	i2c_write(0x04); // Set the address pointer to 4 (hours) by sending 04h.
	i2c_stop();
	i2c_start(0xA3); // Send a RESTART condition or STOP followed by START and end the slave address for read (A3h).
	return (i2c_read(0) & 0b00111111); // Read hours.
}

uint8_t get_day(void)
{
	i2c_start(0xA2); // Send a START condition and the slave address for write (A2h).
	i2c_write(0x05); // Set the address pointer to 5 (days) by sending 05h.
	i2c_stop();
	i2c_start(0xA3); // Send a RESTART condition or STOP followed by START and end the slave address for read (A3h).
	return (i2c_read(0) & 0b00111111); // Read days.
}

uint8_t get_month(void)
{
	i2c_start(0xA2); // Send a START condition and the slave address for write (A2h).
	i2c_write(0x07); // Set the address pointer to 7 (months) by sending 07h.
	i2c_stop();
	i2c_start(0xA3); // Send a RESTART condition or STOP followed by START and end the slave address for read (A3h).
	return (i2c_read(0) & 0b00011111); // Read months.
}

uint8_t get_year(void)
{
	i2c_start(0xA2); // Send a START condition and the slave address for write (A2h).
	i2c_write(0x08); // Set the address pointer to 8 (years) by sending 08h.
	i2c_stop();
	i2c_start(0xA3); // Send a RESTART condition or STOP followed by START and end the slave address for read (A3h).
	return (i2c_read(0)); // Read years.
}

void	print_time(void)
{
	uint8_t hours = get_hour();
	uint8_t minutes = get_minute();
	i2c_stop();
	uint16_t time = hours;
	time = (time<<8);
	time += minutes;
	print_number(bcd_to_decimal(time), 0b00000101);
}

void	print_date(void)
{
	uint16_t date = get_day();
	date = (date<<8);
	date += get_month();
	print_number(bcd_to_decimal(date), 0b00000101);
}

void	print_year(void)
{
	uint16_t year = 0x2000;
	year += get_year();
	print_number(bcd_to_decimal(year), 0);
}

uint8_t time_cmd[21] = {0};
uint8_t lenght_cmd = 0;

uint8_t is_between(uint8_t to_check, uint8_t first, uint8_t last)
{
	return (to_check >= first && to_check <= last);
}

uint8_t check_input_time(void)
{
	uart_printstr(time_cmd);
	uart_printhex(lenght_cmd);
	return (lenght_cmd > 18 &&
			is_between(time_cmd[0], '0', '9') &&
			is_between(time_cmd[1], '0', '9') &&
			time_cmd[2] == 47 &&
			is_between(time_cmd[3], '0', '9') &&
			is_between(time_cmd[4], '0', '9') &&
			time_cmd[5] == 47 &&
			is_between(time_cmd[6], '0', '9') &&
			is_between(time_cmd[7], '0', '9') &&
			is_between(time_cmd[8], '0', '9') &&
			is_between(time_cmd[9], '0', '9') &&
			time_cmd[10] == 32 &&
			is_between(time_cmd[11], '0', '9') &&
			is_between(time_cmd[12], '0', '9') &&
			time_cmd[13] == 58 &&
			is_between(time_cmd[14], '0', '9') &&
			is_between(time_cmd[15], '0', '9') &&
			time_cmd[16] == 58 &&
			is_between(time_cmd[17], '0', '9') &&
			is_between(time_cmd[18], '0', '9'));
}

uint8_t decimal_to_bcd(uint8_t decimal)
{
	uint8_t bcd = decimal / 10;
	bcd = (bcd<<4);
	bcd += decimal % 10;
	return (bcd);
}

void set_date(void)
{
//	uint8_t seconds;
	uint8_t minutes;
//	uint8_t hours;
//	uint8_t days;
//	uint8_t months;
//	uint8_t years;
//
	minutes = time_cmd[15] - '0';
	minutes *= 10;
	minutes += time_cmd[16] - '0';
//
//	seconds = time_cmd[17] - '0';
//	seconds *= 10;
//	seconds += time_cmd[18] - '0';

	i2c_stop();
//	i2c_start(0xA2); // i2c slave address, write
//	i2c_write(0x03); // word address 0, next bytes are data
//	i2c_write(minutes); // set minutes to 45
//	i2c_write(0x00); // timer switched off
//	i2c_stop();

	//set_RTC(decimal_to_bcd(seconds), decimal_to_bcd(minutes), 0x14, 0x28, 0x04, 0x24);
	set_RTC(0x00, 0x06, 0x14, 0x28, 0x04, 0x24);

}

void manage_input(void)
{
	uint8_t c = UDR0;
	uart_tx(c);
	if (c == '\r')
	{
		if (!check_input_time())
		{
			uart_printstr("\r\nERROR, invalid input\r\n");
		}
		else {
			set_date();
		}
		lenght_cmd = 0;
		time_cmd[0] = 0;
		return ;
	}
	else if (lenght_cmd > 20)
	{
		uart_printstr("\r\nERROR, date too long\r\n");
		lenght_cmd = 0;
		time_cmd[0] = 0;
	}
	else
	{
//		uart_tx(UDR0);
		time_cmd[lenght_cmd++] = c;
		time_cmd[lenght_cmd] = 0;
	}
}

ISR(USART_RX_vect)
{
	manage_input();
}

int main(void)
{
	SET_BITS(SREG, (1<<7));
	uart_init();
	SET_BITS(UCSR0B, (1<<RXCIE0));
	

	i2c_init();
	init_seven_segments();
	//set_RTC(0x00, 0x06, 0x14, 0x28, 0x04, 0x24);
	set_RTC(0x00, 0x06, 0x14, 0x28, 0x04, 0x24);
	while (1)
	{
		print_time();
//		_delay_ms(10);
//		print_date();
//		print_year();
//		print_number(bcd_to_decimal(get_month()), 0b00000000);

	}
	return (0);
}
