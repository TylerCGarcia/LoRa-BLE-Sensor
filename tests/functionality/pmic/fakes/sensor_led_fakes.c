#include "sensor_led_fakes.h"

DEFINE_FAKE_VALUE_FUNC(bool, pmic_device_is_ready, const struct device *);

// Basic LED operations
DEFINE_FAKE_VALUE_FUNC(int, led_on, const struct device *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(int, led_off, const struct device *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(int, led_set_brightness, const struct device *, uint32_t, uint8_t);
DEFINE_FAKE_VALUE_FUNC(int, led_blink, const struct device *, uint32_t, uint32_t, uint32_t);

// Reset all fakes
void sensor_led_fakes_reset(void)
{
    RESET_FAKE(pmic_device_is_ready);
    RESET_FAKE(led_on);
    RESET_FAKE(led_off);
    RESET_FAKE(led_set_brightness);
    RESET_FAKE(led_blink);
}