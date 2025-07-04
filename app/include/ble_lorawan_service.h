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

#include "sensor_lorawan.h"

/* 

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
- lorawan_current_process
- lorawan_commands

To-Do
- Add callbacks for write operations to have other funcitons store the data

*/


/* Define the UUIDs for the LORAWAN service */
#define BT_UUID_LORAWAN_VAL                 BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af40)
#define BT_UUID_LORAWAN_ENABLED_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af41)
#define BT_UUID_LORAWAN_FREQUENCY_VAL       BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af42)
#define BT_UUID_LORAWAN_DEV_EUI_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af43)
#define BT_UUID_LORAWAN_JOIN_EUI_VAL        BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af44)
#define BT_UUID_LORAWAN_APP_KEY_VAL         BT_UUID_128_ENCODE(0x5a8cd4ac, 0x1520, 0x4c9d, 0xb488, 0x12942c26af45)

#define BT_UUID_LORAWAN                     BT_UUID_DECLARE_128(BT_UUID_LORAWAN_VAL)
#define BT_UUID_LORAWAN_ENABLED             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_ENABLED_VAL)
#define BT_UUID_LORAWAN_FREQUENCY           BT_UUID_DECLARE_128(BT_UUID_LORAWAN_FREQUENCY_VAL)
#define BT_UUID_LORAWAN_DEV_EUI             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_DEV_EUI_VAL)
#define BT_UUID_LORAWAN_JOIN_EUI            BT_UUID_DECLARE_128(BT_UUID_LORAWAN_JOIN_EUI_VAL)
#define BT_UUID_LORAWAN_APP_KEY             BT_UUID_DECLARE_128(BT_UUID_LORAWAN_APP_KEY_VAL)


/**
 * @brief Connect LoRaWAN setup configuration to LoRaWAN BLE Service. 
 * 
 * @param setup setup configuration used to join lorawan network
 * @return int 0 if successful connection, < 0 if unsuccessful
 */
int ble_lorawan_service_init(lorawan_setup_t *setup);

#endif