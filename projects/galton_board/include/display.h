#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "config.h"

// Estrutura para controle do display
typedef struct {
    uint8_t buffer[SSD1306_BUF_LEN];
    struct render_area frame_area;
} Display;

// Inicialização
void display_init(Display *disp);

// Controle básico
void display_clear(Display *disp);
void display_update(Display *disp);

// Desenho
void display_draw_string(Display *disp, int x, int y, const char *text);

#endif // DISPLAY_H