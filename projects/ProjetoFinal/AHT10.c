#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "include/aht10.h"
#include "include/GY-33.h"
#include "include/BHT1750.h"
#include "include/soil_sensor.h"
#include "include/mqtt.h"
#include "include/display.h"
#include "include/actuators.h"
#include "pico/cyw43_arch.h"
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

int main()
{
    stdio_init_all();
    init_display();
    connect_to_wifi("MARTINS", "campioto98");
    sleep_ms(5000);
    print_network_info();
    mqtt_setup("bitdog2", "192.168.0.100", "aluno", "2509");
    sleep_ms(3000);
    actuators_init();
    printf("FAN slice = %d\n", pwm_gpio_to_slice_num(FAN_EN_PIN));
    printf("HUM slice = %d\n", pwm_gpio_to_slice_num(HUM_EN_PIN));
    printf("LED slice = %d\n", pwm_gpio_to_slice_num(LED_PWM_PIN));

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

while (true) {
    // Mantém Wi-Fi e MQTT ativos (roda sempre!)
    sleep_ms(50);  // loop leve

    static absolute_time_t last_read = 0;
    if (absolute_time_diff_us(last_read, get_absolute_time()) > 5 * 1000000) {
        last_read = get_absolute_time();

        // --- BH1750 ---
        float lux = bh1750_read_lux();
        if (lux >= 0) {
            char lux_[32];
            snprintf(lux_, sizeof(lux_), "$P2:LUM:%.2f", lux);
            mqtt_comm_publish("bitdoglab2/luminosidade", lux_);
        }

        // --- Soil ---
        float soil = soil_sensor_read_percent();
        char soil_[32];
        snprintf(soil_, sizeof(soil_), "$P2:SOIL:%.2f", soil);
        mqtt_comm_publish("bitdoglab2/umidade/solo", soil_);

        // --- AHT10 ---
        float temp = 0, hum = 0;
        if (aht10_read(I2C_PORT, &temp, &hum)) {
            char umi_[32], temp_[32];
            snprintf(umi_, sizeof(umi_), "$P2:HUM:%.2f", hum);
            snprintf(temp_, sizeof(temp_), "$P2:TEMP:%.2f", temp);

            mqtt_comm_publish("bitdoglab2/umidade/ar", umi_);
            mqtt_comm_publish("bitdoglab2/temperatura", temp_);
        }

        // Controle
        control_all(
            hum_min, hum_max, hum,
            temp_min, temp_max, temp,
            lum_min, lum_max, lux,
            soil_min, soil_max, soil
        );
    }
    cyw43_arch_poll();

}
}
