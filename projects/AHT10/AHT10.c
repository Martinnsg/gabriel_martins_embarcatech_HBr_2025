#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/aht10.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

int main() {
    stdio_init_all();

    // Inicializa I2C
    i2c_init(I2C_PORT, 100 * 1000);  // 100kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    aht10_init(I2C_PORT);

    float temp, hum;

    while (true) {
        if (aht10_read(I2C_PORT, &temp, &hum)) {
            printf("Temp: %.2f °C | Hum: %.2f %%\n", temp, hum);
        } else {
            printf("Erro ao ler AHT10\n");
        }
        sleep_ms(2000);
    }
}
