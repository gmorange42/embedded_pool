#ifndef MAIN_C
#define MAIN_C

#include <avr/io.h>
#include <util/delay.h>

#define SET_BIT(reg, pos) (reg |= (1<<pos));
#define CLR_BIT(reg, pos) (reg &= ~(1<<pos));
#define SWI_BIT(reg, pos) (reg ^= (1<<pos));
#define CHK_BIT(reg, pos) (reg & (1<<pos))

#define D1 0
#define SW1 2

#endif
