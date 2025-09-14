#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include "include/actuators.h"

// ------------------ INICIALIZAÇÃO ------------------
void actuators_init() {
    // FAN
    gpio_set_function(FAN_EN_PIN, GPIO_FUNC_PWM);
    gpio_init(FAN_IN1_PIN); gpio_set_dir(FAN_IN1_PIN, GPIO_OUT);
    gpio_init(FAN_IN2_PIN); gpio_set_dir(FAN_IN2_PIN, GPIO_OUT);
    gpio_put(FAN_IN1_PIN, 1);  // sentido fixo
    gpio_put(FAN_IN2_PIN, 0);

    // HUM
    gpio_set_function(HUM_EN_PIN, GPIO_FUNC_PWM);
    gpio_init(HUM_IN3_PIN); gpio_set_dir(HUM_IN3_PIN, GPIO_OUT);
    gpio_init(HUM_IN4_PIN); gpio_set_dir(HUM_IN4_PIN, GPIO_OUT);
    gpio_put(HUM_IN3_PIN, 1);  // sentido fixo
    gpio_put(HUM_IN4_PIN, 0);

    // LED
    gpio_set_function(LED_PWM_PIN, GPIO_FUNC_PWM);

    // Bomba
    gpio_init(PUMP_PIN);
    gpio_set_dir(PUMP_PIN, GPIO_OUT);
    gpio_put(PUMP_PIN, 0);

    // Inicializa PWM (8 bits)
    uint slice_fan = pwm_gpio_to_slice_num(FAN_EN_PIN);
    uint slice_hum = pwm_gpio_to_slice_num(HUM_EN_PIN);
    uint slice_led = pwm_gpio_to_slice_num(LED_PWM_PIN);

    pwm_set_wrap(slice_fan, 255);
    pwm_set_wrap(slice_hum, 255);
    pwm_set_wrap(slice_led, 255);

    pwm_set_enabled(slice_fan, true);
    pwm_set_enabled(slice_hum, true);
    pwm_set_enabled(slice_led, true);
}

// ------------------ FUNÇÃO DE CONTROLE ------------------
uint16_t control_by_range(float value, float min, float max) {
    if (value <= min) return 255; // abaixo do mínimo → 100% PWM
    if (value >= max) return 0;   // acima do máximo → 0% PWM
    float duty = (max - value) / (max - min); 
    return (uint16_t)(duty * 255.0f);
}

void control_hum(float hum_min, float hum_max, float hum_current) {
    uint16_t level = control_by_range(hum_current, hum_min, hum_max);
    pwm_set_gpio_level(HUM_EN_PIN, level);
}

void control_fan(float temp_min, float temp_max, float temp_current) {
    uint16_t level = control_by_range(temp_current, temp_min, temp_max);
    pwm_set_gpio_level(FAN_EN_PIN, level);
}

void control_led(float lux_min, float lux_max, float lux_current) {
    uint16_t level = control_by_range(lux_current, lux_min, lux_max);
    pwm_set_gpio_level(LED_PWM_PIN, level);
}

void control_pump(float soil_min, float soil_max, float soil_current) {
    if (soil_current < soil_min)
        gpio_put(PUMP_PIN, 1);
    else if (soil_current > soil_max)
        gpio_put(PUMP_PIN, 0);
}

// ------------------ CONTROLE CENTRAL ------------------
void control_all(
    float hum_min, float hum_max, float hum_current,
    float temp_min, float temp_max, float temp_current,
    float lux_min, float lux_max, float lux_current,
    float soil_min, float soil_max, float soil_current
) {
    control_hum(hum_min, hum_max, hum_current);
    control_fan(temp_min, temp_max, temp_current);
    control_led(lux_min, lux_max, lux_current);
    control_pump(soil_min, soil_max, soil_current);
}