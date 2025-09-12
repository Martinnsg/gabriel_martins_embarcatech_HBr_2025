#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <stdbool.h>

// Estrutura simples de PID
typedef struct {
    float kp, ki, kd;
    float prev_error;
    float integral;
} PIDController;

// Inicialização dos atuadores (pinos, PWM, etc)
void actuators_init(void);

// Inicialização do controlador PID
void pid_init(PIDController *pid, float kp, float ki, float kd);

// Cálculo do PID
float pid_compute(PIDController *pid, float setpoint, float measured, float dt);

// Controle do ventilador via PWM e PID usando ponte H
void control_fan_pwm_pid(PIDController *pid, float temp_setpoint, float temp_current, float dt, bool forward);

// Controle do umidificador via PWM e PID usando ponte H
void control_humidifier_pwm_pid(PIDController *pid, float hum_setpoint, float hum_current, float dt, bool forward);

// Controle da bomba (on/off)
void control_pump(float soil_min, float soil_max, float soil_current);

// Funções para setar direção (caso queira usar diretamente)
void fan_set_direction(bool forward);
void hum_set_direction(bool forward);

#endif // ACTUATORS_H