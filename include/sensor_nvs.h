/**
 * @file sensor_nvs.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor information and configurations that
 * are stored in the non-volatile storage (NVS) of the device. nrf52840_partition gives 32KB for storage_partition.
 * @version 0.1
 * @date 2025-05-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_NVS_H
#define SENSOR_NVS_H

#include <zephyr/fs/nvs.h>

#define SENSOR_NVS_MAX_SIZE 32

enum sensor_nvs_address {
    SENSOR_NVS_ADDRESS_0,
    SENSOR_NVS_ADDRESS_1,
    SENSOR_NVS_ADDRESS_2,
    SENSOR_NVS_ADDRESS_3,
    SENSOR_NVS_ADDRESS_4,
    SENSOR_NVS_ADDRESS_5,
    SENSOR_NVS_ADDRESS_6,
    SENSOR_NVS_ADDRESS_7,
    SENSOR_NVS_ADDRESS_8,
    SENSOR_NVS_ADDRESS_9,
    SENSOR_NVS_ADDRESS_LIMIT
};

int sensor_nvs_setup(void);

int sensor_nvs_write(enum sensor_nvs_address address, const void *data, size_t length);

int sensor_nvs_read(enum sensor_nvs_address address, void *data, size_t length);

#endif