#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdbool.h>

// Definições dos pinos para ponte H do ventilador
#define FAN_PWM_PIN     2   // PWM (ENA)
#define FAN_IN1_PIN     5   // Direção IN1
#define FAN_IN2_PIN     6   // Direção IN2

// Definições dos pinos para ponte H do umidificador
#define HUM_PWM_PIN     3   // PWM (ENB)
#define HUM_IN1_PIN     7   // Direção IN3
#define HUM_IN2_PIN     8   // Direção IN4

#define PUMP_PIN        4   // Bomba simples (on/off)

// Estrutura simples de PID
typedef struct {
    float kp, ki, kd;
    float prev_error;
    float integral;
} PIDController;

void pid_init(PIDController *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}

float pid_compute(PIDController *pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;
    pid->prev_error = error;
    return pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
}

void actuators_init() {
    // Ventilador
    gpio_set_function(FAN_PWM_PIN, GPIO_FUNC_PWM);
    gpio_init(FAN_IN1_PIN); gpio_set_dir(FAN_IN1_PIN, GPIO_OUT);
    gpio_init(FAN_IN2_PIN); gpio_set_dir(FAN_IN2_PIN, GPIO_OUT);

    // Umidificador
    gpio_set_function(HUM_PWM_PIN, GPIO_FUNC_PWM);
    gpio_init(HUM_IN1_PIN); gpio_set_dir(HUM_IN1_PIN, GPIO_OUT);
    gpio_init(HUM_IN2_PIN); gpio_set_dir(HUM_IN2_PIN, GPIO_OUT);

    // Bomba
    gpio_init(PUMP_PIN);
    gpio_set_dir(PUMP_PIN, GPIO_OUT);
    gpio_put(PUMP_PIN, 0);

    // Inicializa PWM para FAN e HUMIDIFIER
    uint slice_fan = pwm_gpio_to_slice_num(FAN_PWM_PIN);
    uint slice_hum = pwm_gpio_to_slice_num(HUM_PWM_PIN);
    pwm_set_wrap(slice_fan, 255);
    pwm_set_wrap(slice_hum, 255);
    pwm_set_enabled(slice_fan, true);
    pwm_set_enabled(slice_hum, true);
}

// Função para setar direção do ventilador
void fan_set_direction(bool forward) {
    gpio_put(FAN_IN1_PIN, forward ? 1 : 0);
    gpio_put(FAN_IN2_PIN, forward ? 0 : 1);
}

// Função para setar direção do umidificador
void hum_set_direction(bool forward) {
    gpio_put(HUM_IN1_PIN, forward ? 1 : 0);
    gpio_put(HUM_IN2_PIN, forward ? 0 : 1);
}

// Controle do ventilador via PWM e PID usando ponte H
void control_fan_pwm_pid(PIDController *pid, float temp_setpoint, float temp_current, float dt, bool forward) {
    float output = pid_compute(pid, temp_setpoint, temp_current, dt);
    if (output < 0) output = 0;
    if (output > 255) output = 255;
    fan_set_direction(forward);
    pwm_set_gpio_level(FAN_PWM_PIN, (uint16_t)output);
}

// Controle do umidificador via PWM e PID usando ponte H
void control_humidifier_pwm_pid(PIDController *pid, float hum_setpoint, float hum_current, float dt, bool forward) {
    float output = pid_compute(pid, hum_setpoint, hum_current, dt);
    if (output < 0) output = 0;
    if (output > 255) output = 255;
    hum_set_direction(forward);
    pwm_set_gpio_level(HUM_PWM_PIN, (uint16_t)output);
}

// Controle da bomba permanece simples (on/off)
void control_pump(float soil_min, float soil_max, float soil_current) {
    if (soil_current < soil_min) {
        gpio_put(PUMP_PIN, 1); 
    } else if (soil_current > soil_max) {
        gpio_put(PUMP_PIN, 0); 
    }
}