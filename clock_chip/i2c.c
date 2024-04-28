#include "i2c.h"
#include <util/twi.h>

void error(uint8_t* msg)
{
	uart_printstr(msg);
	uart_printstr(" [");
	print_hex_value(TWSR);
	uart_printstr("]\n\r");
}

void print_hex_value(uint8_t n)
{
    char c = n >> 4;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
    c = n & 0x0F;
    uart_tx(c < 10 ? c + '0' : c - 10 + 'A');
}

void i2c_init(void)
{
	TWBR = ((F_CPU / 100000) - 16) / 2; // Set the Value of the TWI Bit Rate Register
}

//void i2c_start(uint8_t addr, uint8_t wr)
void i2c_start(uint8_t addr)
{
	//WR -> [Read = 1] [Write = 0]
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))) {} // Wait for TWINT Flag set. Indicate the START condition
					// has been transmitted
	if (TW_STATUS != TW_START && TW_STATUS != 0x10) // Check value of TWI Status Register.
							// Mask prescaler bits.
	{
		error("i2c start() => TW_STATUS FAILED (TW_START)");
		return;
	}
//	TWDR = (addr<<1) | wr; // Load Device Adrress into TWDR Register and Read/Write bit.
	TWDR = addr; // Load Device Adrress into TWDR Register and Read/Write bit.
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the SLA_W / SLA_R has
					// been transmitted, and ACK/NACK has been received.
	if (TW_STATUS != TW_MT_SLA_ACK && 
			TW_STATUS != TW_MR_SLA_ACK &&
			TW_STATUS != TW_MR_SLA_NACK) // Check value of TWI Status Register.
						    // Mask prescaler bits.If status different
						    // from MT_SLA_ACK or MR_SLA_ACK go to ERROR.
	{
		error("i2c start() => TW_STATUS FAILED (MT_MT_SLA_ACK)");
		return;
	}
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); // Send STOP condition
}

void i2c_write(unsigned char data)
{
	TWDR = data; // Load data into TWDR Register.
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that data has
					// been transmitted, and ACK/NACK has been received.
	if (TW_STATUS != TW_MT_DATA_ACK) // Check value of TWI Status Register. Mask prescaler bits.
	{				 // If status different from MT_SLA_ACK go to ERROR.
		error("i2c write() => TW_STATUS FAILED");
		return;
	}
}

uint8_t i2c_read(uint8_t ack)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (ack<<TWEA); // Set TWEA (ACK/NACK) and clear TWINT bit in
						     // TWCR to start transmission of address
	while (!(TWCR & (1<<TWINT))) {} // Wait for Flag set. This indicates that the data has
					// been received, and ACK/NACK has been transmitted.
	if (TW_STATUS != TW_MR_DATA_ACK &&
			TW_STATUS != TW_MR_DATA_NACK) // Check value of TWI Status Register.
	{					      // Mask prescaler bits. If status different
						      // from MT_DATA_ACK or MT_DATA_NACK go to ERROR.
		error("i2c read() => TW_STATUS FAILED");
		return (TWDR);
	}
	return (TWDR);
//	print_hex_value(TWDR); // Print Data
}
