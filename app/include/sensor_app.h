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

#define SENSOR_VOLTAGE_NAME_LENGTH      20
#define SENSOR_TYPE_NAME_LENGTH         20

// In sensor_app.h
typedef enum sensor_app_state{
    SENSOR_APP_STATE_CONFIGURATION,    // Device is in configuration mode (BLE active, waiting for setup)
    SENSOR_APP_STATE_RUNNING,         // Device is actively reading sensors and transmitting
    SENSOR_APP_STATE_ERROR,           // Error state
};

enum sensor_nvs_address {
    SENSOR_NVS_ADDRESS_DEVICE_NAME,
    SENSOR_NVS_ADDRESS_APP_STATE,
    SENSOR_NVS_ADDRESS_LORAWAN_ENABLED,
    SENSOR_NVS_ADDRESS_LORAWAN_FREQUENCY,
    SENSOR_NVS_ADDRESS_LORAWAN_DEV_EUI,
	SENSOR_NVS_ADDRESS_LORAWAN_JOIN_EUI,
	SENSOR_NVS_ADDRESS_LORAWAN_APP_KEY,
    SENSOR_NVS_ADDRESS_LORAWAN_DEV_NONCE,
    SENSOR_NVS_ADDRESS_LORAWAN_JOIN_ATTEMPTS,
    SENSOR_NVS_ADDRESS_LORAWAN_SEND_ATTEMPTS,
    SENSOR_NVS_ADDRESS_SENSOR_1_ENABLED,
    SENSOR_NVS_ADDRESS_SENSOR_1_POWER,
    SENSOR_NVS_ADDRESS_SENSOR_1_TYPE,
    SENSOR_NVS_ADDRESS_SENSOR_1_FREQUENCY,
    SENSOR_NVS_ADDRESS_SENSOR_2_ENABLED,
    SENSOR_NVS_ADDRESS_SENSOR_2_POWER,
    SENSOR_NVS_ADDRESS_SENSOR_2_TYPE,
    SENSOR_NVS_ADDRESS_SENSOR_2_FREQUENCY,
	SENSOR_NVS_ADDRESS_LIMIT,
};

typedef struct {
    /* Current state of the app */
    enum sensor_app_state state;
    /* Tells whether or not to connect network during configuration */
    uint8_t connect_network_during_configuration;
    /* Sensor type used for sensor 1 */
    enum sensor_types sensor_1_type;
    /* Sensor power name used for sensor 1 */
    char sensor_1_type_name[SENSOR_TYPE_NAME_LENGTH];
    /* Sensor type used for sensor 2 */
    enum sensor_types sensor_2_type;
    /* Sensor type name used for sensor 2 */
    char sensor_2_type_name[SENSOR_TYPE_NAME_LENGTH];
    /* Power voltage used for sensor 1 */
    enum sensor_voltage sensor_1_voltage;
    /* Power voltage name used for sensor 1 */
    char sensor_1_voltage_name[SENSOR_VOLTAGE_NAME_LENGTH];
    /* Power voltage used for sensor 2 */
    enum sensor_voltage sensor_2_voltage;
    /* Power voltage name used for sensor 2 */
    char sensor_2_voltage_name[SENSOR_VOLTAGE_NAME_LENGTH];
    /* Whether sensor 1 is enabled */
    uint8_t is_sensor_1_enabled;
    /* Whether sensor 2 is enabled */
    uint8_t is_sensor_2_enabled;
    /* Frequency of sensor 1 */
    uint8_t sensor_1_frequency;
    /* Frequency of sensor 2 */
    uint8_t sensor_2_frequency;
    /* Data from sensor 1 */
    uint8_t *sensor_1_latest_data;
    /* Time since the latest data was received from sensor 1 */
    uint32_t sensor_1_latest_data_timestamp;
    /* Data from sensor 2 */
    uint8_t *sensor_2_latest_data;
    /* Time since the latest data was received from sensor 2 */
    uint32_t sensor_2_latest_data_timestamp;
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