#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h"

// Configuração dos pinos do Joystick (BitDogLab)
#define JOYSTICK_X_PIN 26  // GP26 (ADC0)
#define JOYSTICK_Y_PIN 27  // GP27 (ADC1)
#define JOYSTICK_BUTTON_PIN 22  // GP22

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

// Função para inicializar o joystick
void init_joystick() {
    adc_init();
    
    // Configura os pinos X e Y
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    // Configura o botão do joystick
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
}

// Função para ler um canal ADC (0-3.3V -> 0-4095)
uint16_t read_adc(uint pin) {
    adc_select_input(pin - 26); // Converte GPIO para número ADC (0-3)
    return adc_read();
}

// Função para atualizar o display
void update_display(uint16_t x_val, uint16_t y_val, bool button_pressed) {
    char line1[20];
    char line2[20];
    char line3[20];
    
    memset(buf, 0, SSD1306_BUF_LEN);
    
    snprintf(line1, sizeof(line1), "X: %4d", x_val);
    snprintf(line2, sizeof(line2), "Y: %4d", y_val);
    snprintf(line3, sizeof(line3), "Btn: %s", button_pressed ? "ON " : "OFF");
    
    WriteString(buf, 0, 0, "Joystick BitDogLab");
    WriteString(buf, 0, 16, line1);
    WriteString(buf, 0, 32, line2);
    WriteString(buf, 0, 48, line3);
    
    render(buf, &frame_area);
}

int main() {
    stdio_init_all();
    init_joystick();
    init_display();
    
    while (true) {
        // Lê os valores do joystick
        uint16_t x_value = read_adc(JOYSTICK_X_PIN);
        uint16_t y_value = read_adc(JOYSTICK_Y_PIN);
        bool button_state = !gpio_get(JOYSTICK_BUTTON_PIN); // Inverte porque é pull-up
        
        // Atualiza o display
        update_display(x_value, y_value, button_state);
        
        sleep_ms(100); // Taxa de atualização de 10Hz
    }
    
    return 0;
}