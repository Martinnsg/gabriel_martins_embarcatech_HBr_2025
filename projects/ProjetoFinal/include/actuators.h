#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "pico/stdlib.h"
#include <stdbool.h>

// ------------------ PINOS ------------------
#define FAN_EN_PIN      18   // PWM para FAN
#define FAN_IN1_PIN     19   // direção FAN
#define FAN_IN2_PIN     20   // direção FAN (sempre invertido)

#define HUM_EN_PIN      16   // PWM para HUM
#define HUM_IN3_PIN     17   // direção HUM
#define HUM_IN4_PIN     28   // direção HUM (sempre invertido)

#define LED_PWM_PIN     9    // PWM LED
#define PUMP_PIN        10   // Bomba on/off

// ------------------ API ------------------

// Inicializa todos os atuadores (PWM + GPIO)
void actuators_init(void);

// Direção dos motores (se necessário inverter)

// Controle individual
void control_hum(float hum_min, float hum_max, float hum_current);
void control_fan(float temp_min, float temp_max, float temp_current);
void control_led(float lux_min, float lux_max, float lux_current);
void control_pump(float soil_min, float soil_max, float soil_current);

// Controle centralizado (todos de uma vez)
void control_all(
    float hum_min, float hum_max, float hum_current,
    float temp_min, float temp_max, float temp_current,
    float lux_min, float lux_max, float lux_current,
    float soil_min, float soil_max, float soil_current
);

#endif // ACTUATORS_H
