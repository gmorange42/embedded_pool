#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

int main(void)
{
	i2c_init();
	i2c_start(0b00100000, 0);  // send address in write mode
	i2c_write(6); // Configuration port 0 first
	i2c_write(0b11110111); // Set register 0 pin 3 to 0 (output)
	i2c_write(0b11111111);
	i2c_stop();
	while (1)
	{
		i2c_start(0b00100000, 0);  // send address in write mode
		i2c_write(2); // Output port 0 first
		i2c_write(0b11110111); // Set register 0 pin 3 to 0 (HIGH)
		i2c_write(0b11111111);
		i2c_stop();
		_delay_ms(500);
		i2c_start(0b00100000, 0);  // send address in write mode
		i2c_write(2); // Output port 0 first
		i2c_write(0b11111111); // Set register 0 pin 3 to 1 (LOW)
		i2c_write(0b11111111);
		i2c_stop();
		_delay_ms(500);
	}
	return (0);
}
