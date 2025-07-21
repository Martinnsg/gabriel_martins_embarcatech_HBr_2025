#include "include/aht10.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

void aht10_init(i2c_inst_t *i2c) {
    uint8_t cmd[] = {0xE1, 0x08, 0x00};  // Calibração
    i2c_write_blocking(i2c, AHT10_ADDR, cmd, 3, false);
    sleep_ms(20);
}

bool aht10_read(i2c_inst_t *i2c, float *temp, float *hum) {
    uint8_t trigger[] = {0xAC, 0x33, 0x00};
    i2c_write_blocking(i2c, AHT10_ADDR, trigger, 3, false);
    sleep_ms(80);  // Aguarda medição

    uint8_t data[6];
    if (i2c_read_blocking(i2c, AHT10_ADDR, data, 6, false) != 6)
        return false;

    // Verifica se bit de ocupação está setado
    if (data[0] & 0x80) return false;

    uint32_t raw_hum = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4));
    uint32_t raw_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);

    *hum = ((float)raw_hum * 100) / 1048576;
    *temp = ((float)raw_temp * 200 / 1048576) - 50;

    return true;
}
