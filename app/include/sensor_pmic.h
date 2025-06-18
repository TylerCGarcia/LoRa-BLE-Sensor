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
#include <stdint.h>
#include <zephyr/drivers/sensor.h>

typedef struct {
    float percentage;
    float voltage;
    float current;
    float temp;
    uint32_t chg_status;
    uint32_t chg_error;
    bool vbus_present;
} pmic_sensor_status_t;

/**
 * @brief Structure containing function pointers for PMIC operations
 */
typedef struct {
    bool (*device_is_ready)(const struct device *dev);
    int (*led_on)(const struct device *dev, uint32_t led);
    int (*led_off)(const struct device *dev, uint32_t led);
    int (*sensor_status_get)(const struct device *dev, pmic_sensor_status_t *status);
} sensor_pmic_ops_t;

/**
 * @brief Initialize the PMIC with custom operations
 * 
 * @param ops Pointer to operations structure. If NULL, default operations will be used
 * @return int 0 on success, negative error code on failure
 */
int sensor_pmic_init_with_ops(const sensor_pmic_ops_t *ops);

/**
 * @brief Initialize the PMIC, with default operations.
 * 
 * @return int 0 on success, negative error code on failure
 */
int sensor_pmic_init(void);

/**
 * @brief Turn on the LED
 * 
 * @return int 0 on success, negative error code on failure
 */
int sensor_pmic_led_on(void);

/**
 * @brief Turn off the LED
 * 
 * @return int 0 on success, negative error code on failure
 */
int sensor_pmic_led_off(void);

/**
 * @brief Get the status of the PMIC sensors
 * 
 * @param status Pointer to the status structure
 * @return int 0 on success, negative error code on failure
 */
int sensor_pmic_status_get(pmic_sensor_status_t *status);

#endif