#include <stdio.h>
#include "pico/stdlib.h"
#include "audio_capture.h"      // Coleta de áudio (ADC)
#include "audio_output.h"     // Reprodução via PWM
#include "io_manager.h"       // Botões e LEDs


int main() {
    stdio_init_all();              // Inicializa entrada/saída padrão (debug)
    
    adc_configure();             // Configura ADC no pino do microfone
    configure_pwm_output();        // Configura PWM para saída no buzzer
    setup_io();                    // Inicializa GPIOs para botões e LEDs

    while (true) {
        control_led(LED_STATUS_GREEN, false);
        control_led(LED_STATUS_BLUE, false);
        
        if (check_button(RECORD_BUTTON_PIN)) {
            control_led(LED_STATUS_GREEN, true);
            capture_audio();       // Grava áudio no buffer
            control_led(LED_STATUS_GREEN, false);
            control_led(LED_STATUS_RED, true);
        }

        if (check_button(PLAY_BUTTON_PIN)) {
            control_led(LED_STATUS_RED, false);
            control_led(LED_STATUS_BLUE, true);
            output_audio_stream(sample_data, SAMPLE_COUNT);
            control_led(LED_STATUS_BLUE, false);
        }

        sleep_ms(100); // Delay simples para debounce
    }

    return 0;
}
