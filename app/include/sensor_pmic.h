/**
 * @file sensor_pmic.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of the PMIC and its configuration.
 * @version 0.1
 * @date 2025-06-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_PMIC_H
#define SENSOR_PMIC_H

#include <zephyr/device.h>
#include <stdbool.h>

/**
 * @brief Initialize the PMIC, with its charging and led configuration.
 * 
 */
int sensor_pmic_init(void);

/**
 * @brief Turn on the LED
 * 
 */
int sensor_pmic_led_on(void);

/**
 * @brief Turn off the LED
 * 
 */
int sensor_pmic_led_off(void);

/**
 * @brief Check if the PMIC device is ready. This is defined as weak to be overridden during testing.
 * 
 * @param dev 
 * @return true if the device is ready, false otherwise
 */
__attribute__((weak)) bool pmic_device_is_ready(const struct device *dev);

#endif