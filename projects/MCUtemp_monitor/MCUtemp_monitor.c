#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

float read_internal_temp() {
    adc_init();
    
    // Habilita o sensor de temperatura
    adc_set_temp_sensor_enabled(true);
    
    // Seleciona o canal do sensor de temperatura (ADC4)
    adc_select_input(4);
    
    uint16_t raw = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = raw * conversion_factor;
    
    // Converte para temperatura (fórmula do datasheet)
    float temp_c = 27.0f - (voltage - 0.706f) / 0.001721f;
    
    return temp_c;
}

int main() {

    stdio_init_all();

    printf("\n=== Monitor de Temperatura RP2040 ===\n");
    
    while (true) {
        float temperature = read_internal_temp();
        printf("Temperatura: %.2f C\n", temperature);
        sleep_ms(1000); // Atualiza a cada 1 segundo
    }
    
    return 0;
}