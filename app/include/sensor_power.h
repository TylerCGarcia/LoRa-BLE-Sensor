/**
 * @file sensor_power.h
 * @author Tyler Garcia
 * @brief This is a library for sensor power configurations and control.
 * This is manages the power configuration, pin configuration, and control of sensor power.
 * @version 0.1
 * @date 2025-05-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_POWER_H
#define SENSOR_POWER_H

#include "sensor_id.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

#define OUTUT_READ_DIVIDER_HIGH     100
#define OUTUT_READ_DIVIDER_LOW      13

/**
 * @brief Sensor power configuration
 */
typedef struct {
    /* Sensor power configurationid. */
    enum sensor_power_id power_id;
    /* GPIO spec for the boost enable pin. */
    struct gpio_dt_spec boost_en; 
    /* GPIO spec for the boost control 1 pin. */
    struct gpio_dt_spec boost_ctrl1;
    /* GPIO spec for the boost control 2 pin. */
    struct gpio_dt_spec boost_ctrl2;
    /* LDO device for the sensor. */
    const struct device *ldo_dev;
    /* ADC spec for reading the output voltage. */
    const struct adc_dt_spec output_read;
    /* Delay in ms to wait after setting the voltage to allow the voltage to stabilize. */
    uint32_t delay_ms;
} sensor_power_config_t;

/**
 * @brief Initialize a sensor_power_setup. This sets the id of the output, the gpios beings used and the regulator device being used.
 * After configurations are set, the sensors output is set OFF.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return int -1 if error setting up power system or adc
 */
int sensor_power_init(sensor_power_config_t *config);

/**
 * @brief Get the sensor voltage that is set for an output.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return enum sensor_voltage for selected sensor configuration
 */
enum sensor_voltage get_sensor_output(sensor_power_config_t *config);

/**
 * @brief Set the sensor voltage for an output, enabling/disabling the correct regulators, and setting the correct gpios.
 * When setting the voltage to OFF the voltage may take 1-2 seconds to fully turn off, due to capacitors on the output.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @param voltage enum sensor_voltage setting selected
 * @return int 0 if successful, sets voltage to OFF if invalid voltage input
 */
int set_sensor_output(sensor_power_config_t *config, enum sensor_voltage voltage);

/**
 * @brief Read the voltage output of the selected sensor power configuration. Takes into account resistor divider on output.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return float output of the sensor power system
 */
float read_sensor_output(sensor_power_config_t *config);

/**
 * @brief 
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @param voltage enum sensor_voltage setting selected
 * @param accepted_error error in percent that the device is allowed to be off by
 * @return int 0 if successful, -1 if not
 */
int validate_output(sensor_power_config_t *config, enum sensor_voltage voltage, uint8_t accepted_error);

#endif

