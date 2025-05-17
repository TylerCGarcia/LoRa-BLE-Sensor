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

// In sensor_app.h
typedef enum sensor_app_state{
    SENSOR_APP_STATE_UNINITIALIZED = 0,
    SENSOR_APP_STATE_CONFIGURATION,    // Device is in configuration mode (BLE active, waiting for setup)
    SENSOR_APP_STATE_RUNNING,         // Device is actively reading sensors and transmitting
    SENSOR_APP_STATE_ERROR,           // Error state
};

typedef struct {
    enum sensor_app_state state;
} sensor_app_config_t;

/**
 * @brief Initialize the sensor app, handle anything that needs to be initialized for the app such as timers, nvs, etc.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_init(void);

/**
 * @brief Start the BLE advertising and services.
 * 
 * @return int 0 on success, -1 on failure
 */
int sensor_app_ble_start(void);

#endif