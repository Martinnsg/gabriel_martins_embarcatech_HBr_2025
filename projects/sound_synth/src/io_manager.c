// io_manager.c

#include "include/io_manager.h"
#include "pico/stdlib.h"

// Inicializa os pinos dos botões e LEDs do sistema
void setup_io() {
    // Configuração dos botões (modo entrada com pull-up)
    gpio_init(RECORD_BUTTON_PIN);
    gpio_set_dir(RECORD_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RECORD_BUTTON_PIN);

    gpio_init(PLAY_BUTTON_PIN);
    gpio_set_dir(PLAY_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(PLAY_BUTTON_PIN);

    // LEDs configurados como saída
    gpio_init(LED_STATUS_GREEN);
    gpio_init(LED_STATUS_BLUE);
    gpio_init(LED_STATUS_RED);

    gpio_set_dir(LED_STATUS_GREEN, GPIO_OUT);
    gpio_set_dir(LED_STATUS_BLUE, GPIO_OUT);
    gpio_set_dir(LED_STATUS_RED, GPIO_OUT);
}

// Retorna verdadeiro se o botão estiver ativado (nível baixo)
bool check_button(uint32_t button_pin) {
    return gpio_get(button_pin) == 0;
}

// Define o estado de um LED: ligado ou desligado
void control_led(uint32_t led_pin, bool enable) {
    gpio_put(led_pin, enable);
}
