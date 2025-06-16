// In tests/fakes/sensor_pmic_fakes.c
#include "sensor_pmic_fakes.h"

// Define fake functions
DEFINE_FAKE_VALUE_FUNC(int, sensor_pmic_init);
DEFINE_FAKE_VALUE_FUNC(int, sensor_pmic_led_on);
DEFINE_FAKE_VALUE_FUNC(int, sensor_pmic_led_off);

// Reset all fakes
void sensor_pmic_fakes_reset(void)
{
    RESET_FAKE(sensor_pmic_init);
    RESET_FAKE(sensor_pmic_led_on);
    RESET_FAKE(sensor_pmic_led_off);
}