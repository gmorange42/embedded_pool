#ifndef RGB_H
#define RGB_H

#include <avr/io.h>
#include "bit_wise.h"

void init_rgb(void);
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
void wheel(uint8_t pos);

#endif
