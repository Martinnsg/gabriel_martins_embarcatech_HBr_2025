// audio_capture.c

#include "audio_capture.h"

#include "hardware/adc.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

// Array que armazenará as amostras digitalizadas do sinal de áudio
uint8_t sample_data[SAMPLE_COUNT];

// Configurações iniciais do ADC (canal, pino e ativação)
void adc_configure() {
    adc_init();                     // Habilita o ADC interno do chip
    adc_gpio_init(28);             // Usa o pino 28 como entrada analógica
    adc_select_input(2);           // Seleciona o canal 2 (relacionado ao GPIO28)
}

// Captura um conjunto de amostras do sinal analógico
void capture_audio() {
    for (int index = 0; index < SAMPLE_COUNT; index++) {
        uint16_t adc_value = adc_read();  // Leitura do ADC (12 bits)
        
        // Conversão simples para 8 bits: elimina os 4 bits inferiores
        sample_data[index] = (uint8_t)(adc_value >> 4);
        
        // Taxa de amostragem: 8 kHz → 125 µs entre leituras
        sleep_us(125);
    }
}
