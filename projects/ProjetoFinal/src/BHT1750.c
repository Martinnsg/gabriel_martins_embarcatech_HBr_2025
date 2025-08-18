#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/BHT1750.h"

// Endereço padrão do BH1750
#define BH1750_ADDR 0x23

// Comandos do BH1750
#define BH1750_POWER_ON   0x01
#define BH1750_RESET      0x07
#define BH1750_CONT_HRES  0x10  // Continuous H-Resolution Mode

// Pinos do I2C (ajuste conforme seu hardware)
#define I2C_PORT i2c0
#define SDA_PIN  0
#define SCL_PIN  1

void bh1750_init() {

    i2c_init(I2C_PORT, 100 * 1000); // 100kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    uint8_t buf[1];

    // Liga o sensor
    buf[0] = BH1750_POWER_ON;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, buf, 1, false);

    // Reset
    buf[0] = BH1750_RESET;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, buf, 1, false);

    // Modo contínuo de alta resolução
    buf[0] = BH1750_CONT_HRES;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, buf, 1, false);
}

float bh1750_read_lux() {
    uint8_t data[2];
    int res = i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false);

    if (res < 0) {
        return -1; // erro na leitura
    }

    // Conversão: valor raw (16 bits) / 1.2 = lux
    uint16_t raw = (data[0] << 8) | data[1];
    return (float)raw / 1.2f;
}


