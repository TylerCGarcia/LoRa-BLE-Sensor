/**
 * @file sensor_data.h
 * @author Tyler Garcia
 * @brief This is a library for sensor data configurations and collection.
 * This is manages the sensor type, pin configuration, and reading of sensor data.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "sensor_id.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

/**
 * @brief Regulator identifiers
 */
typedef enum sensor_types {
    /* No sensor configuration is set. */
    NULL_SENSOR = 0,
    /* Sensor is set to read from voltage_read pin. */
    VOLTAGE_SENSOR,
    /* Sensor is set to read from current_read pin. */
    CURRENT_SENSOR,
    /* Sensor is set to read low pulses from d1 pin. */
    PULSE_SENSOR,
    /* Used to denote size of enum. */
    SENSOR_TYPE_LIMIT
};

/**
 * @brief Sensor data pin configuration
 */
typedef struct {
    /* Identification number of sensor. */
    enum sensor_id id;
    /* First Digital Pin. */
    struct gpio_dt_spec d1;
    /* Second Digital Pin. */ 
    struct gpio_dt_spec d2;
    /* ADC spec for voltage sensor. */
    const struct adc_dt_spec voltage_read;
    /* ADC spec for current sensor. */
    const struct adc_dt_spec current_read;
} sensor_data_config_t;

#define VOLTAGE_READ_DIVIDER_HIGH     100
#define VOLTAGE_READ_DIVIDER_LOW      13
#define CURRENT_READ_RESISTOR         50
#define PULSE_DEBOUNCE_MS             50

/**
 * @brief Setup sensor for sensor_type with hardware configuration 
 * 
 * @param config sensor hardware configuration
 * @param sensor_type type of sensor to be set to
 * @return int 0 if successful, < 0 otherwise
 */
int sensor_data_setup(sensor_data_config_t *config, enum sensor_types sensor_type);

/**
 * @brief Get the sensor type set for a hardware configuration
 * 
 * @param config sensor hardware configuration
 * @return enum sensor_types current sensor type set
 */
enum sensor_types get_sensor_data_setup(sensor_data_config_t *config);

/**
 * @brief Get the sensor voltage reading for a given sensor, takes into account 
 * the VOLTAGE_READ_DIVIDER_HIGH and VOLTAGE_READ_DIVIDER_LOW values
 * 
 * @param config sensor hardware configuration
 * @return float voltage on sensor voltage_read pin, -1 if sensor is not configured to VOLTAGE_SENSOR
 */
float get_sensor_voltage_reading(sensor_data_config_t *config);

/**
 * @brief Get the sensor current reading for a given sensor in milliamperes, takes 
 * into account the CURRENT_READ_RESISTOR
 * 
 * @param config sensor hardware configuration
 * @return float current on sensor current_read pin in mA, -1 if sensor is not configured to CURRENT_SENSOR
 */
float get_sensor_current_reading(sensor_data_config_t *config);

/**
 * @brief Get the current number of pulses captured on since initialization or last reset. 
 * 
 * @param config sensor hardware configuration
 * @return int number of pulses, -1 if sensor is not configured to PULSE_SENSOR
 */
int get_sensor_pulse_count(sensor_data_config_t *config);

/**
 * @brief Reset the pulse count for a given sensor to 0.
 * 
 * @param config sensor hardware configuration
 * @return int 0 if successful, -1 if sensor is not configured to PULSE_SENSOR
 */
int reset_sensor_pulse_count(sensor_data_config_t *config);

#endif
