#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define TCS34725_ADDRESS 0x29

#define TCS34725_COMMAND_BIT 0x80
#define TCS34725_ENABLE 0x00
#define TCS34725_ENABLE_PON 0x01
#define TCS34725_ENABLE_AEN 0x02
#define TCS34725_CDATAL 0x14  // Clear data low byte

// Matriz de correção de cor aproximada (ajuste se quiser)
float ccm[3][3] = {
    {1.0479f, -0.0229f, -0.0502f},
    {-0.0296f, 1.0010f, 0.0151f},
    {0.0009f, -0.0092f, 0.7519f}
};

void tcs34725_enable() {
    uint8_t enable_cmd[] = {TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON};
    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, enable_cmd, 2, false);
    sleep_ms(10);
    enable_cmd[1] = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, enable_cmd, 2, false);
    sleep_ms(700);  // Tempo de integração
}

void read_color_data(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue) {
    uint8_t reg = TCS34725_COMMAND_BIT | TCS34725_CDATAL;
    uint8_t buffer[8];

    i2c_write_blocking(I2C_PORT, TCS34725_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, TCS34725_ADDRESS, buffer, 8, false);

    *clear = (buffer[1] << 8) | buffer[0];
    *red   = (buffer[3] << 8) | buffer[2];
    *green = (buffer[5] << 8) | buffer[4];
    *blue  = (buffer[7] << 8) | buffer[6];
}

void calibrate_and_print(uint16_t clear, uint16_t r, uint16_t g, uint16_t b) {
    if (clear == 0) {
        printf("Clear é zero, não é possível normalizar.\n");
        return;
    }
    printf("Cor antes de normalizar : R: %d  G:%d  B:%d\n", r,g,b);
    float R = (float)r / clear;
    float G = (float)g / clear;
    float B = (float)b / clear;
    printf("Cor normalizada : R: %.2f  G:%.2f  B:%.2f\n", R, G, B);

    // Aplica matriz de correção
    float Rc = ccm[0][0]*R + ccm[0][1]*G + ccm[0][2]*B;
    float Gc = ccm[1][0]*R + ccm[1][1]*G + ccm[1][2]*B;
    float Bc = ccm[2][0]*R + ccm[2][1]*G + ccm[2][2]*B;

    // Garante limites entre 0 e 1
    if (Rc < 0) Rc = 0; if (Rc > 1) Rc = 1;
    if (Gc < 0) Gc = 0; if (Gc > 1) Gc = 1;
    if (Bc < 0) Bc = 0; if (Bc > 1) Bc = 1;

    // Correção gama (gamma = 2.2)
    Rc = powf(Rc, 1.0f/2.2f);
    Gc = powf(Gc, 1.0f/2.2f);
    Bc = powf(Bc, 1.0f/2.2f);

    uint8_t r8 = (uint8_t)(Rc * 255);
    uint8_t g8 = (uint8_t)(Gc * 255);
    uint8_t b8 = (uint8_t)(Bc * 255);

    printf("Cor calibrada HEX: #%02X%02X%02X\n", r8, g8, b8);
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Inicializando TCS34725...\n");
    tcs34725_enable();

    uint16_t clear, red, green, blue;

    while (true) {
        read_color_data(&clear, &red, &green, &blue);
        printf("C: %u, R: %u, G: %u, B: %u\n", clear, red, green, blue);
        calibrate_and_print(clear, red, green, blue);
        sleep_ms(1000);
    }

    return 0;
}
