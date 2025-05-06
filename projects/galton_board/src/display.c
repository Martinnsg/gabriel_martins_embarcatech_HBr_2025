#include "include/display.h"
#include "include/ssd1306.h"
#include <string.h>

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