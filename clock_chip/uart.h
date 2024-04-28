#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define UBRR_VALUE  (uint16_t)((float)((F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1)

void uart_init(void);
void uart_tx(char c);
void uart_printstr(uint8_t* str);
char uart_rx(void);
void uart_printhex(uint8_t n);
void uart_print_number(uint16_t val);

#endif
