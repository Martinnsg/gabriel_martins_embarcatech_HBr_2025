#include "include/display.h"
#include "include/ssd1306.h"
#include <string.h>
#include <stdio.h>
void display_init(Display *disp) {
    // Inicializa I2C
    i2c_init(i2c1, I2C_CLOCK_SPEED);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa display
    SSD1306_init();

    // Configura área de renderização
    disp->frame_area.start_col = 0;
    disp->frame_area.end_col = SSD1306_WIDTH - 1;
    disp->frame_area.start_page = 0;
    disp->frame_area.end_page = SSD1306_NUM_PAGES - 1;

    calc_render_area_buflen(&disp->frame_area);
    display_clear(disp);
}

void display_clear(Display *disp) {
    memset(disp->buffer, 0, SSD1306_BUF_LEN);
}

void display_update(Display *disp) {
    render(disp->buffer, &disp->frame_area);
}

void display_draw_string(Display *disp, int x, int y, const char *text) {
    WriteString(disp->buffer, (int16_t)x, (int16_t)y, (char *)text);
}

<<<<<<< HEAD
void display_draw_circle(Display *disp, int x, int y, int radius, bool on) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                SetPixel(disp->buffer, x + dx, y + dy, on);
            }
        }
    }
}

void display_draw_line(Display *disp, int x0, int y0, int x1, int y1, bool on) {
    DrawLine(disp->buffer, x0, y0, x1, y1, on);
}
=======
void display_draw_ball(Display *disp, int x, int y) {
    SetPixel(disp->buffer,x, y, true);
    SetPixel(disp->buffer,x - 1, y, true);
    SetPixel(disp->buffer,x + 1, y, true);
}

void display_draw_total_count(Display *disp, int count) {
    char msg[20];
    snprintf(msg, sizeof(msg), "%d", count);
    WriteString(disp->buffer, 0, 0, msg); // topo da tela
}

void display_draw_pins(Display *disp, int num_pins) {
    int base_x = 64;  // Posição inicial em x
    int base_y = 10;  // Posição inicial em y

    // Define o número de linhas de pinos
    int line_total = 7;

    for (int line = 0; line < line_total; line++) {
        int num_pin = line + 1;
        int y_distance = base_y + line * 8;
        int x_distance = 8;

        for (int pino = 0; pino < num_pin; pino++) {
            int x = base_x + pino * x_distance - line * x_distance / 2; 
            int y = y_distance;

            SetPixel(disp->buffer,x, y, true);
        }
    }
}
>>>>>>> 4416be1 (connect to wifi ok)
