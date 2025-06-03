/**
 * @file ble_sensor_service.h
 * @author Tyler Garcia
 * @brief This is a library to handle the BLE services for the sensor.
 * @version 0.1
 * @date 2025-05-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BLE_SENSOR_SERVICE_H
#define BLE_SENSOR_SERVICE_H

#include "sensor_app.h"
/* 
- Service DEVICE
- device_name (r/w)
- hardware_version (r)
- firmware_version (r)
- device_type (r)
- Commands (w)
    - clear and resetup nvs (w)
    - restart device (w)
- number of sensors (r)
- number of power configurations (r)
- Battery Status (r)
    - Battery State (r)
    - Battery Level (r)
    - Battery Voltage (r)
    - Battery Current (r)
    - Battery Temperature (r)
    - Battery Percentage (r)
*/

/* Define the UUIDs for the Sensor service */
#define BT_UUID_SENSOR_VAL                  BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c0)
#define BT_UUID_SENSOR_STATE_VAL            BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c1)

#define BT_UUID_SENSOR1_ENABLED_VAL         BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c2)
#define BT_UUID_SENSOR1_CONFIG_VAL          BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c3)
#define BT_UUID_SENSOR1_PWR_CONFIG_VAL      BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c4)
#define BT_UUID_SENSOR1_DATA_FREQ_VAL       BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c5)
#define BT_UUID_SENSOR1_DATA_VAL            BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c6)
#define BT_UUID_SENSOR1_DATA_TIMESTAMP_VAL  BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c7)

#define BT_UUID_SENSOR2_ENABLED_VAL         BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c8)
#define BT_UUID_SENSOR2_CONFIG_VAL          BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c9)
#define BT_UUID_SENSOR2_PWR_CONFIG_VAL      BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2ca)
#define BT_UUID_SENSOR2_DATA_FREQ_VAL       BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2cb)
#define BT_UUID_SENSOR2_DATA_VAL            BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2cc)
#define BT_UUID_SENSOR2_DATA_TIMESTAMP_VAL  BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2cd)

/* Attach the UUIDs for the Sensor service */
#define BT_UUID_SENSOR                      BT_UUID_DECLARE_128(BT_UUID_SENSOR_VAL)
#define BT_UUID_SENSOR_STATE                BT_UUID_DECLARE_128(BT_UUID_SENSOR_STATE_VAL)

#define BT_UUID_SENSOR1_ENABLED             BT_UUID_DECLARE_128(BT_UUID_SENSOR1_ENABLED_VAL)
#define BT_UUID_SENSOR1_CONFIG              BT_UUID_DECLARE_128(BT_UUID_SENSOR1_CONFIG_VAL)
#define BT_UUID_SENSOR1_PWR_CONFIG          BT_UUID_DECLARE_128(BT_UUID_SENSOR1_PWR_CONFIG_VAL)
#define BT_UUID_SENSOR1_DATA_FREQ           BT_UUID_DECLARE_128(BT_UUID_SENSOR1_DATA_FREQ_VAL)
#define BT_UUID_SENSOR1_DATA                BT_UUID_DECLARE_128(BT_UUID_SENSOR1_DATA_VAL)
#define BT_UUID_SENSOR1_DATA_TIMESTAMP      BT_UUID_DECLARE_128(BT_UUID_SENSOR1_DATA_TIMESTAMP_VAL)

#define BT_UUID_SENSOR2_ENABLED             BT_UUID_DECLARE_128(BT_UUID_SENSOR2_ENABLED_VAL)
#define BT_UUID_SENSOR2_CONFIG              BT_UUID_DECLARE_128(BT_UUID_SENSOR2_CONFIG_VAL)
#define BT_UUID_SENSOR2_PWR_CONFIG          BT_UUID_DECLARE_128(BT_UUID_SENSOR2_PWR_CONFIG_VAL)
#define BT_UUID_SENSOR2_DATA_FREQ           BT_UUID_DECLARE_128(BT_UUID_SENSOR2_DATA_FREQ_VAL)
#define BT_UUID_SENSOR2_DATA                BT_UUID_DECLARE_128(BT_UUID_SENSOR2_DATA_VAL)
#define BT_UUID_SENSOR2_DATA_TIMESTAMP      BT_UUID_DECLARE_128(BT_UUID_SENSOR2_DATA_TIMESTAMP_VAL)

/**
 * @brief Initialize the BLE sensor service
 * 
 * @param config Pointer to the sensor app config
 * @return int 0 if successful, < 0 if unsuccessful
 */
int ble_sensor_service_init(sensor_app_config_t *config);

#endif