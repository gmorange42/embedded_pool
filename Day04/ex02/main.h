#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define SET_BITS(reg, mask) (reg |= mask);
#define CLR_BITS(reg, mask) (reg &= ~mask)
#define SWI_BITS(reg, mask) (reg ^= mask);
#define CHK_BITS(reg, mask) ((reg & mask) == mask)

void error(uint8_t* msg);
void i2c_init(void);
void i2c_start(uint8_t addr, uint8_t wr);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_read(void);
void uart_init(void);
void uart_tx(char c);
void uart_printstr(uint8_t* str);
char uart_rx(void);
void print_hex_value(uint8_t n);

#endif
