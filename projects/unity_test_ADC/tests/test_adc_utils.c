// tests/test_adc_utils.c
#include "unity.h"
#include "adc_utils.h"

void setUp(void) {}
void tearDown(void) {}

void test_adc_to_celsius_known_value(void) {
    // 0.706V corresponde a 27 °C
    uint16_t adc_val = (uint16_t)((0.706 / 3.3) * 4095);
    float temperature = adc_to_celsius(adc_val);

    TEST_ASSERT_FLOAT_WITHIN(0.5, 27.0f, temperature);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_known_value);
    return UNITY_END();
}
