#ifndef DISPLAY_THRESHOLDS_H
#define DISPLAY_THRESHOLDS_H

#include <stdint.h>
#include "ssd1306.h"

// Inicializa o display OLED
void init_display(void);

// Atualiza o display com os limiares de sensores
// t_min, t_max -> Temperatura
// h_min, h_max -> Umidade do ar
// s_min, s_max -> Umidade do solo
// l_min, l_max -> Luminosidade
void update_thresholds_display(float t_min, float t_max,
                               float h_min, float h_max,
                               float s_min, float s_max,
                               float l_min, float l_max);

void clear_display(void);                               
#endif // DISPLAY_THRESHOLDS_H
