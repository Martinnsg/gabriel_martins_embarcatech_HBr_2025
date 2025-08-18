#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/GY-33.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define TCS34725_ADDRESS 0x29

#define TCS34725_COMMAND_BIT 0x80
#define TCS34725_ENABLE 0x00
#define TCS34725_ENABLE_PON 0x01
#define TCS34725_ENABLE_AEN 0x02
#define TCS34725_CDATAL 0x14  // Clear data low byte

void tcs34725_enable() {

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    // Liga o sensor (PON + AEN)
    printf("Ativando sensor \n");
    uint8_t enable_cmd[] = {TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON};
    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, enable_cmd, 2, false);
    sleep_ms(10);
    enable_cmd[1] = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, enable_cmd, 2, false);
    sleep_ms(700);  // Espera tempo de integração
}

void read_color_data(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue) {
    uint8_t reg = TCS34725_COMMAND_BIT | TCS34725_CDATAL;
    uint8_t buffer[8];
    //printf("Lendo sensor \n");

    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, TCS34725_ADDRESS, buffer, 8, false);

    *clear = (buffer[1] << 8) | buffer[0];
    *red   = (buffer[3] << 8) | buffer[2];
    *green = (buffer[5] << 8) | buffer[4];
    *blue  = (buffer[7] << 8) | buffer[6];

}

void print_normalized_color_hex(uint16_t nclear, uint16_t nred, uint16_t ngreen, uint16_t nblue) {
    if (nclear == 0) {
        printf("Clear é zero, não é possível normalizar.\n");
        return;
    }

    float r_norm = (float)nred / nclear;
    float g_norm = (float)ngreen / nclear;
    float b_norm = (float)nblue / nclear;
    //printf("Valores normalizados: R: %.2f, G: %.2f, B: %.2f\n", r_norm, g_norm, b_norm);
    // Normaliza para o maior valor 1.0 se ultrapassar
    float max_val = r_norm;
    if (g_norm > max_val) max_val = g_norm;
    if (b_norm > max_val) max_val = b_norm;

    if (max_val > 1.0f) {
        r_norm /= max_val;
        g_norm /= max_val;
        b_norm /= max_val;
    }

    uint8_t r_8bit = (uint8_t)(r_norm * 255);
    uint8_t g_8bit = (uint8_t)(g_norm * 255);
    uint8_t b_8bit = (uint8_t)(b_norm * 255);

    printf("Cor HEX: #%02X%02X%02X\n", r_8bit, g_8bit, b_8bit);
}
