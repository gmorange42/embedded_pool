#ifndef MAIN_C
#define MAIN_C

#include <avr/io.h>
#include <util/delay.h>

#define SET_BIT(reg, pos) (reg |= (1<<pos));
#define CLR_BIT(reg, pos) (reg &= ~(1<<pos));
#define SWI_BIT(reg, pos) (reg ^= (1<<pos));
#define CHK_BIT(reg, pos) (reg & (1<<pos))

#define D1 0
#define D2 1
#define D3 2
#define D4 4
#define SW1 2
#define SW2 4

void increment(void);
void decrement(void);

#endif
