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

/**
 * @brief Setup NVS for use. This must be called before any other NVS functions. This mounts the NVS partition and initializes it.
 * 
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_setup(void);

/**
 * @brief Write given data buffer to NVS with given address.
 * 
 * @param address address of NVS to write to
 * @param data pointer to data buffer to write
 * @param length length of data to write
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_write(enum sensor_nvs_address address, const void *data, size_t length);

/**
 * @brief Read data from NVS with given address, data is written to the provided buffer.
 * 
 * @param address address of NVS to read from
 * @param data pointer to data buffer to write
 * @param length length of data to read
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_read(enum sensor_nvs_address address, void *data, size_t length);

/**
 * @brief Delete data from NVS with given address.
 * 
 * @param address address of NVS to delete
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_delete(enum sensor_nvs_address address);

/**
 * @brief Clear all data from NVS. NVS must be setup again after clearing.
 * 
 * @return int 0 on success, -ERRNO errno code if error
 */
int sensor_nvs_clear(void);

#endif