#include "sensor_led_fakes.h"

bool device_is_ready_wrapper(const struct device *dev)
{
    return true;
}

// Wrapper functions to match the expected function signatures
int led_on_wrapper(const struct device *dev, uint32_t led)
{
    return 0;
}

int led_off_wrapper(const struct device *dev, uint32_t led)
{
    return 0;
}

int sensor_status_get_wrapper(const struct device *dev, pmic_sensor_status_t *status)
{
    return 0;
}