#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

#include "pico/stdlib.h"

// Inicializa o ADC para o pino onde o sensor está conectado
void soil_sensor_init(uint adc_pin);

// Faz a leitura do valor do sensor (retorna de 0 a 4095)
uint16_t soil_sensor_read_raw(void);

// Converte o valor para porcentagem (0% a 100%)
float soil_sensor_read_percent(void);

#endif
