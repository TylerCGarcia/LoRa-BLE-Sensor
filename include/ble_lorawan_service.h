/**
 * @file ble_lorawan_service.h
 * @author Tyler Garcia
 * @brief This is a library to handle the BLE services for the LORAWAN network.
 * This service is used for configuration and debugging of the LORAWAN network.
 * @version 0.1
 * @date 2025-05-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BLE_LORAWAN_SERVICE_H
#define BLE_LORAWAN_SERVICE_H


/* 
- Service DEVICE_SETTINGS
- device_name
- hardware_version
- firmware_version
- device_type


- Service LORAWAN
- lorawan_dev_eui
- lorawan_app_eui
- lorawan_net_key
- lorawan_connection_status
- lorawan_transmission_frequency
- lorawan_time_since_last_transmission
- lorawan_number_of_attempts_for_tranmissions
- lorawan_last_transmission_status
- lorawan_last_transmission_data

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


/* Define the UUIDs for the LORAWAN service */
#define BT_UUID_LORAWAN_VAL                 BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af40)
#define BT_UUID_LORAWAN_DEV_EUI_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af41)
#define BT_UUID_LORAWAN_APP_EUI_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af42)
#define BT_UUID_LORAWAN_NET_KEY_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af43)

#define BT_UUID_LORAWAN                     BT_UUID_DECLARE_128(BT_UUID_LORAWAN_VAL)
#define BT_UUID_LORAWAN_DEV_EUI             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_DEV_EUI_VAL)
#define BT_UUID_LORAWAN_APP_EUI             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_APP_EUI_VAL)
#define BT_UUID_LORAWAN_NET_KEY             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_NET_KEY_VAL)


#endif