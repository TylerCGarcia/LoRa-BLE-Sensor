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


/* 
- Service DEVICE
- device_name (r/w)
- hardware_version (r)
- firmware_version (r)
- device_type (r)
- Battery State (r)
- Battery Level (r)
- Battery Voltage (r)
- Battery Current (r)
- Battery Temperature (r)
- Battery Percentage (r)
- Battery Status (r)


- Service SENSOR
- sensor_power1_configuration
- sensor_power2_configuration

- sensor1_configuration_name
- sensor1_read_timing
- sensor1_data (string to keep all data the same format)
- sensor1_data_time_since_last_capture (uint32_t)

- sensor2_configuration_name
- sensor2_read_timing
- sensor2_data (string to keep all data the same format)
- sensor2_data_time_since_last_capture (uint32_t)

*/

/* Define the UUIDs for the Sensor service */
#define BT_UUID_SENSOR_VAL BT_UUID_128_ENCODE(0x07de1ad6, 0x4f7a, 0x4156, 0x9836, 0x77690b6ed2c0)

#define BT_UUID_SENSOR BT_UUID_DECLARE_128(BT_UUID_SENSOR_VAL)




#endif