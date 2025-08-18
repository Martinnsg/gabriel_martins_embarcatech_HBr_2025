#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/aht10.h"
#include "include/GY-33.h"
#include "include/BHT1750.h"




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

    printf("Inicializando TCS34725...\n");
    tcs34725_enable();
    float temp, hum;
    uint16_t clear, red, green, blue;
    bh1750_init();

    while (true) {
        // Leitura sensor de umidade e temperatura
        if (aht10_read(I2C_PORT, &temp, &hum)) {
            printf("Temp: %.2f °C | Hum: %.2f %%\n", temp, hum);
        } else {
            printf("Erro ao ler AHT10\n");
        }
        
        // Leitura sensor de cor
        read_color_data(&clear, &red, &green, &blue);
        printf("C: %u, R: %u, G: %u, B: %u\n", clear, red, green, blue);
        print_normalized_color_hex(clear, red, green, blue);

        // Leitura sensor de luminosidade
        float lux = bh1750_read_lux();
        if (lux >= 0) {
            printf("Luminosidade: %.2f lux\n", lux);
        } else {
            printf("Erro na leitura do BH1750\n");
        }
        sleep_ms(2000);
    }
   
}

