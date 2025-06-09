// io_manager.h

#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"

// Inicializa os periféricos de entrada e saída (botões e LEDs)
void setup_io(void);

// Verifica se um botão foi pressionado (nível baixo)
bool check_button(uint32_t button_pin);

// Liga ou desliga um LED
void control_led(uint32_t led_pin, bool enable);

// Mapeamento dos pinos dos botões
#define RECORD_BUTTON_PIN 5
#define PLAY_BUTTON_PIN   6

// Mapeamento dos pinos dos LEDs
#define LED_STATUS_GREEN  11
#define LED_STATUS_BLUE   12
#define LED_STATUS_RED    13

#endif
