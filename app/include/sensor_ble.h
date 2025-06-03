/**
 * @file sensor_ble.h
 * @author Tyler Garcia
 * @brief This is a library to handle the BLE functionality for the sensor.
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_BLE_H
#define SENSOR_BLE_H

#include <stdint.h>
#include <zephyr/bluetooth/bluetooth.h>


/* Structure to hold the BLE setup. */
typedef struct {
    /* Advertising options. */
    enum bt_le_adv_opt adv_opt;
    /* Advertising interval min in milliseconds. */
    uint32_t adv_interval_min_ms;
    /* Advertising interval max in milliseconds. */
    uint32_t adv_interval_max_ms;
    /* Advertising name. */
    char adv_name[32];
} ble_config_t;

/* Structure to hold the advertising intervals using zephyr codes(not milliseconds). */
typedef struct {
    /* Advertising interval min in zephyr codes(not milliseconds). */
    uint32_t min;
    /* Advertising interval max in zephyr codes(not milliseconds). */
    uint32_t max;
} adv_interval_t;

/**
 * @brief Setup the BLE device, with advertisement and connection parameters.
 * 
 * @param config The configuration of the BLE device.
 * @return int 0 on success, negative on failure.
 */
int ble_setup(ble_config_t *config);

/**
 * @brief Check if the BLE device is advertising.
 * 
 * @return int 1 if advertising, 0 if not.
 */
int is_ble_advertising(void);

/**
 * @brief Change the name of the BLE device.
 * 
 * @param config The configuration of the BLE device.
 * @return int 0 on success, negative on failure.
 */
int ble_change_name(ble_config_t *config);

/**
 * @brief Get the advertising interval of the BLE device.
 * 
 * @return adv_interval_t The advertising interval of the BLE device.
 */
adv_interval_t get_ble_adv_interval(void);

#endif