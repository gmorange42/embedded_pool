#ifndef I2C_H
#define I2C_H
#include <avr/io.h>
#include "uart.h"

void error(uint8_t* msg);
void print_hex_value(uint8_t n);
void i2c_init(void);
//void i2c_start(uint8_t addr, uint8_t wr);
void i2c_start(uint8_t addr);
void i2c_stop(void);
void i2c_write(unsigned char data);
uint8_t i2c_read(uint8_t ack);

#endif
