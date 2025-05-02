// drivers/adc_utils.c
#include "adc_utils.h"

float adc_to_celsius(uint16_t adc_val) {
    float voltage = (adc_val * 3.3f) / 4095.0f;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}
