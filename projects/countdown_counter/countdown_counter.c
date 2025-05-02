#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "include/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Definição pinos botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Definição pinos I2C
const uint I2C_SDA_PIN = 14;
const uint I2C_SCL_PIN = 15;

// Variáveis globais
volatile int counter = 0;
volatile int button_b_presses = 0;
volatile bool counting_active = false;
volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;
struct repeating_timer timer;

// Buffer e área de renderização do display
uint8_t buf[SSD1306_BUF_LEN];
struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
};

// Protótipos de função
void button_a_isr(uint gpio, uint32_t events);
void button_b_isr(uint gpio, uint32_t events);
void update_display();
bool countdown_callback(struct repeating_timer *t);

int main() {
    stdio_init_all();

    // Inicialização I2C
    i2c_init(i2c1, SSD1306_I2C_CLK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicialização do display
    SSD1306_init();
    calc_render_area_buflen(&frame_area);
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    // Inicialização dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_isr);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    // Estado inicial
    counter = 0;
    button_b_presses = 0;
    counting_active = false;
    update_display();

    // Loop principal
    while (true) {
        if (button_a_pressed) {
            button_a_pressed = false;
            
            // Cancela timer anterior se existir
            cancel_repeating_timer(&timer);
            
            // Reinicia o contador
            counter = 9;
            button_b_presses = 0;
            counting_active = true;
            update_display();
            
            // Configura o timer para contagem regressiva (1 segundo)
            add_repeating_timer_ms(1000, countdown_callback, NULL, &timer);
        }
        
        if (button_a_pressed && counting_active) {
            button_b_pressed = false;
            button_b_presses++;
            update_display();
        }
        
        tight_loop_contents();
    }
    
    return 0;
}

// Interrupção para o Botão A
void button_a_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        sleep_ms(50);
        button_a_pressed = true;
    }
}

// Interrupção para o Botão B
void button_b_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_B_PIN) {
        sleep_ms(50);
        button_b_pressed = true;
    }
}

// Callback para a contagem regressiva
bool countdown_callback(struct repeating_timer *t) {
    if (counting_active) {
        counter--;
        update_display();
        
        if (counter <= 0) {
            counting_active = false;
            return false; // Para o timer
        }
    }
    return true; // Continua o timer
}

void update_display() {
    char line1[20];
    char line2[20];
    
    // Limpa o buffer do display
    memset(buf, 0, SSD1306_BUF_LEN);

    // Atualiza o display OLED
    WriteString(buf, 0, 0, line1);
    WriteString(buf, 0, 16, line2);
    render(buf, &frame_area);

}
