#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "config.h"

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
void display_draw_circle(Display *disp, int x, int y, int radius, bool on);
void display_draw_line(Display *disp, int x0, int y0, int x1, int y1, bool on);


void display_draw_ball(Display *disp, int x, int y);
void display_draw_total_count(Display *disp, int count);
void display_draw_pins(Display *disp, int num_pins);
#endif // DISPLAY_H