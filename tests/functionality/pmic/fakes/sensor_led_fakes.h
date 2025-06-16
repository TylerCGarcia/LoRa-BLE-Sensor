#ifndef SENSOR_LED_FAKES_H
#define SENSOR_LED_FAKES_H

#include <zephyr/fff.h>
#include <zephyr/device.h>
// #include <zephyr/drivers/led.h>
#include <stdint.h>
#include <stdbool.h>

DECLARE_FAKE_VALUE_FUNC(bool, pmic_device_is_ready, const struct device *);

// Basic LED operations
DECLARE_FAKE_VALUE_FUNC(int, led_on, const struct device *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(int, led_off, const struct device *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(int, led_set_brightness, const struct device *, uint32_t, uint8_t);
DECLARE_FAKE_VALUE_FUNC(int, led_blink, const struct device *, uint32_t, uint32_t, uint32_t);

// Reset all fakes
void sensor_led_fakes_reset(void);

#endif