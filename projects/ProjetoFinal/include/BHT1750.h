#ifndef BHT1750_H
#define BHT1750_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

void bh1750_init();
float bh1750_read_lux();

#endif
