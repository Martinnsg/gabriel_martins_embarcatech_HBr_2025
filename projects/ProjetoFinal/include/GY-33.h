#ifndef GY33_H
#define GY33_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

void tcs34725_enable();
void read_color_data(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue);
void print_normalized_color_hex(uint16_t nclear, uint16_t nred, uint16_t ngreen, uint16_t nblue);

#endif
