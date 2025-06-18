#ifndef SENSOR_LED_FAKES_H
#define SENSOR_LED_FAKES_H

#include <zephyr/device.h>
#include <stdint.h>
#include <stdbool.h>
#include <zephyr/drivers/sensor.h>
#include <sensor_pmic.h>

bool device_is_ready_wrapper(const struct device *dev);
int led_on_wrapper(const struct device *dev, uint32_t led);
int led_off_wrapper(const struct device *dev, uint32_t led);
int sensor_status_get_wrapper(const struct device *dev, pmic_sensor_status_t *status);

#endif