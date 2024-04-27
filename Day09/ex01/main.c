#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "uart.h"

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask);
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

int main(void)
{
	uint8_t counter = 0;

	// Set pins for LEDs and btn SW3
	i2c_init();
	i2c_start(0b00100000, 0);  // send address in write mode
	i2c_write(6); // Set command Configuration port 0 first
	i2c_write(0b11110001); // Set pins 2 3 4 to 0 (output)
	i2c_write(0b11111111);
	i2c_stop();
	while (1)
	{
		// WRITE PART
		i2c_start(0b00100000, 0);  // send address in write mode
		i2c_write(2); //Send command Output port 0 first
		i2c_write(~(counter<<1)); // Set pins with the bits of counter 
		i2c_write(0b11111111);
		i2c_stop();

		// READ PART
		i2c_start(0b00100000, 0);  // send address in write mode
		i2c_write(0); // Send command Input port 0
		i2c_start(0b00100000, 1);  // [restart] send address in read mode
		if (!(CHK_BITS(i2c_read(0), (1<<0))))
		{
			counter = (++counter) % 8;
			_delay_ms(200);
		}
		i2c_stop();
	}
	return (0);
}
