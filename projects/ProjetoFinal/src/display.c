#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h"

// Configuração do display OLED
const uint I2C_SDA_PIN = 14;
const uint I2C_SCL_PIN = 15;

// Variáveis globais para o display
uint8_t buf[SSD1306_BUF_LEN];
struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
};

// Função para inicializar o display
void init_display() {
    i2c_init(i2c1, SSD1306_I2C_CLK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    SSD1306_init();
    calc_render_area_buflen(&frame_area);
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);
}

void clear_display() {
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);
}

void update_thresholds_display(float t_min, float t_max,
                               float h_min, float h_max,
                               float s_min, float s_max,
                               float l_min, float l_max) {
    char line1[26];
    char line2[26];
    char line3[26];
    char line4[26];

    // Limpa o buffer
    memset(buf, 0, SSD1306_BUF_LEN);

    // Monta as linhas com floats (1 casa decimal, suporta até 9999.9)
    snprintf(line1, sizeof(line1), "TEMP: %.1f - %.1f", t_min, t_max);
    snprintf(line2, sizeof(line2), "UMID: %.1f - %.1f", h_min, h_max);
    snprintf(line3, sizeof(line3), "SOLO: %.1f - %.1f", s_min, s_max);
    snprintf(line4, sizeof(line4), "LUZ:  %.1f - %.1f", l_min, l_max);

    // Escreve no buffer do display
    WriteString(buf, (SSD1306_WIDTH - (8 * strlen("LIMIARES:"))) / 2, 0, "LIMIARES:");
    WriteString(buf, 0, 16, line1);
    WriteString(buf, 0, 28, line2);
    WriteString(buf, 0, 40, line3);
    WriteString(buf, 0, 52, line4);

    // Renderiza no display
    render(buf, &frame_area);
}
