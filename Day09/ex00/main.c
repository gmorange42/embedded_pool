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
	i2c_write(6); // Configuration port 0
	i2c_write(0b11110111); // Set pin 4 to output
	i2c_write(0b11111111);
	i2c_stop();
	i2c_start(0b00100000, 0);  // send address in write mode
	i2c_write(2); // Output port 0
	i2c_write(0b11110111); // Set pin 4 to output
	i2c_write(0b11111111);
	i2c_stop();
	while (1)
	{}
	return (0);
}
