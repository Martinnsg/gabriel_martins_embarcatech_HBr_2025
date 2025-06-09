// audio_output.c

#include "audio_output.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

// Pino de saída de áudio conectado ao buzzer
#define BUZZER_OUTPUT_PIN 10

void configure_pwm_output() {
    gpio_set_function(BUZZER_OUTPUT_PIN, GPIO_FUNC_PWM);

    uint pwm_slice = pwm_gpio_to_slice_num(BUZZER_OUTPUT_PIN);

    pwm_config pwm_cfg = pwm_get_default_config();
    pwm_config_set_wrap(&pwm_cfg, 255); // 8-bit resolution

    pwm_init(pwm_slice, &pwm_cfg, true);
}

void output_audio_stream(const uint8_t *audio_data, int length) {
    uint pwm_slice = pwm_gpio_to_slice_num(BUZZER_OUTPUT_PIN);

    for (int pos = 0; pos < length; pos++) {
        pwm_set_gpio_level(BUZZER_OUTPUT_PIN, audio_data[pos]);
        sleep_us(125); // 8 kHz sample rate
    }

    // Silencia a saída após reprodução
    pwm_set_gpio_level(BUZZER_OUTPUT_PIN, 0);
}
