/**
 * @file ble_device_service.h
 * @author Tyler Garcia
 * @brief This is a library to handle the BLE services device configuration.
 * It displays the device name, hardware version, firmware version, and battery/charger information.
 * @version 0.1
 * @date 2025-05-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BLE_DEVICE_SERVICE_H
#define BLE_DEVICE_SERVICE_H

#include "sensor_pmic.h"

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

/* Define the UUIDs for the LORAWAN service */
#define BT_UUID_DEVICE_VAL                      BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c90)
#define BT_UUID_DEVICE_NAME_VAL                 BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c91)
#define BT_UUID_DEVICE_HARDWARE_VERSION_VAL     BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c92)
#define BT_UUID_DEVICE_FIRMWARE_VERSION_VAL     BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c93)
#define BT_UUID_DEVICE_BATTERY_PERCENT_VAL      BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c94)
#define BT_UUID_DEVICE_BATTERY_VOLTAGE_VAL      BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c95)
#define BT_UUID_DEVICE_BATTERY_CURRENT_VAL      BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c96)
#define BT_UUID_DEVICE_BATTERY_TEMPERATURE_VAL  BT_UUID_128_ENCODE(0xf983f80d, 0x277a, 0x423a, 0xa122, 0xa37302ff7c97)


#define BT_UUID_DEVICE                     BT_UUID_DECLARE_128(BT_UUID_DEVICE_VAL)
#define BT_UUID_DEVICE_NAME                BT_UUID_DECLARE_128(BT_UUID_DEVICE_NAME_VAL)
#define BT_UUID_DEVICE_HARDWARE_VERSION    BT_UUID_DECLARE_128(BT_UUID_DEVICE_HARDWARE_VERSION_VAL)
#define BT_UUID_DEVICE_FIRMWARE_VERSION    BT_UUID_DECLARE_128(BT_UUID_DEVICE_FIRMWARE_VERSION_VAL)
#define BT_UUID_DEVICE_BATTERY_PERCENT     BT_UUID_DECLARE_128(BT_UUID_DEVICE_BATTERY_PERCENT_VAL)
#define BT_UUID_DEVICE_BATTERY_VOLTAGE     BT_UUID_DECLARE_128(BT_UUID_DEVICE_BATTERY_VOLTAGE_VAL)
#define BT_UUID_DEVICE_BATTERY_CURRENT     BT_UUID_DECLARE_128(BT_UUID_DEVICE_BATTERY_CURRENT_VAL)
#define BT_UUID_DEVICE_BATTERY_TEMPERATURE BT_UUID_DECLARE_128(BT_UUID_DEVICE_BATTERY_TEMPERATURE_VAL)


/**
 * @brief Connect PMIC sensor status to Device BLE Service. 
 * 
 * @param status PMIC sensor status
 * @return int 0 if successful connection, < 0 if unsuccessful
 */
int ble_device_service_init(pmic_sensor_status_t *status);

#endif