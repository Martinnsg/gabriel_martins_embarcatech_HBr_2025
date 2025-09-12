#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/aht10.h"
#include "include/GY-33.h"
#include "include/BHT1750.h"
#include "include/soil_sensor.h"
#include "include/mqtt.h"
#include "include/display.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

int main()
{
    stdio_init_all();
    init_display();
    connect_to_wifi("MARTINS WIFI-2.4", "20025450");
    sleep_ms(5000);
    print_network_info();
    mqtt_setup("bitdog'", "192.168.15.146", "aluno", "2509");
    sleep_ms(3000);

    // Inicializa sensor de umidade no GPIO26 (ADC0)
    soil_sensor_init(26);

    // Inicializa I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Inicializa sensores
    aht10_init(I2C_PORT);
    bh1750_init();
    tcs34725_enable();

    char lux_[32];  // buffer para a string do lux
    char soil_[32]; // buffer para a string do lux
    char umi_[32];  // buffer para a string do lux
    char temp_[32]; // buffer para a string do lux

    while (true)
    {
        sleep_ms(2000);

        // --- BH1750 ---
        float lux = bh1750_read_lux();
        if (lux >= 0)
        {
            // printf("$LUM:%.2f\n", lux);
            snprintf(lux_, sizeof(lux_), "$P2:LUM:%.2f", lux);
            mqtt_comm_publish("bitdoglab2/luminosidade", lux_, strlen(lux_));
        }
        else
        {
            // printf("Erro na leitura do BH1750\n");
        }
        // --- Soil Sensor ---
        float soil = soil_sensor_read_percent();
        snprintf(soil_, sizeof(soil_), "$P2:SOIL:%.2f", soil);
        mqtt_comm_publish("bitdoglab2/umidade/solo", soil_, strlen(soil_));

        // printf("$SOIL:%.2f\n", soil);

        // --- AHT10 (Temp e Umidade do Ar) ---
        float temp = 0, hum = 0;
        if (aht10_read(I2C_PORT, &temp, &hum))
        {
            snprintf(umi_, sizeof(umi_), "$P2:HUM:%.2f", hum);
            snprintf(temp_, sizeof(temp_), "$P2:TEMP:%.2f", temp);

            mqtt_comm_publish("bitdoglab2/umidade/ar", umi_, strlen(umi_));

            mqtt_comm_publish("bitdoglab2/temperatura", temp_, strlen(temp_));

            // printf("$TEMP:%.2f\n", temp);
            // printf("$HUM:%.2f\n", hum);
        }
        else
        {
            // printf("Erro na leitura do AHT10\n");
        }
    }
}