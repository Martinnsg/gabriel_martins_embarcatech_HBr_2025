#ifndef AHT10_H
#define AHT10_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define AHT10_ADDR 0x38

void aht10_init(i2c_inst_t *i2c);
bool aht10_read(i2c_inst_t *i2c, float *temp, float *hum);

#endif
