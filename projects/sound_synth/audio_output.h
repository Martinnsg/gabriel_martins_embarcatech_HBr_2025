// audio_output.h

#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>

// Configura o periférico PWM para saída de áudio
void configure_pwm_output(void);

// Reproduz os dados de áudio via PWM
// - audio_data: ponteiro para as amostras
// - length: número total de amostras
void output_audio_stream(const uint8_t *audio_data, int length);

#endif
