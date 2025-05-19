/**
 * @file sensor_app.h
 * @author Tyler Garcia
 * @brief A library to manage sensor applications. Working between
 * power configurations and data configurations.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_APP_H
#define SENSOR_APP_H

#include "sensor_id.h"
#include <stdint.h>
// In sensor_app.h
typedef enum sensor_app_state{
    SENSOR_APP_STATE_CONFIGURATION,    // Device is in configuration mode (BLE active, waiting for setup)
    SENSOR_APP_STATE_RUNNING,         // Device is actively reading sensors and transmitting
    SENSOR_APP_STATE_ERROR,           // Error state
};

typedef struct {
    /* Current state of the app */
    enum sensor_app_state state;
    /* Frequency of LoRaWAN */
    uint32_t lorawan_frequency;
    /* Tells whether or not to connect lorawan during configuration */
    uint8_t connect_lorawan_during_configuration;
    /* Sensor type used for sensor 1 */
    enum sensor_types sensor_1_type;
    /* Sensor type used for sensor 2 */
    enum sensor_types sensor_2_type;
    /* Power voltage used for sensor 1 */
    enum sensor_voltage sensor_1_voltage;
    /* Power voltage used for sensor 2 */
    enum sensor_voltage sensor_2_voltage;
    /* Whether sensor 1 is enabled */
    uint8_t is_sensor_1_enabled;
    /* Whether sensor 2 is enabled */
    uint8_t is_sensor_2_enabled;
    /* Frequency of sensor 1 */
    uint32_t sensor_1_frequency;
    /* Frequency of sensor 2 */
    uint32_t sensor_2_frequency;
} sensor_app_config_t;

/**
 * @brief Initialize the sensor app, handle anything that needs to be initialized for the app such as timers, nvs, etc.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_init(sensor_app_config_t *config);

/**
 * @brief Execute the sensor app configuration state.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_configuration_state(void);

/**
 * @brief Execute the sensor app running state.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_running_state(void);

/**
 * @brief Execute the sensor app error state.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_error_state(void);

/**
 * @brief Start the BLE advertising and services.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_ble_start(void);

#endif