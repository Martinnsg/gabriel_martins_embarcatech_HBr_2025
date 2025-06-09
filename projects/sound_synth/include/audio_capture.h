// audio_capture.h

#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <stdint.h>
#include <stdbool.h>

// Número total de amostras de áudio (equivalente a 5 segundos a 8 kHz)
#define SAMPLE_COUNT 40000

// Buffer de amostras digitalizadas (definido em audio_capture.c)
extern uint8_t sample_data[SAMPLE_COUNT];

// Inicializa e configura o ADC
void adc_configure(void);

// Executa a aquisição de sinal e armazena os dados no buffer
void capture_audio(void);

#endif
