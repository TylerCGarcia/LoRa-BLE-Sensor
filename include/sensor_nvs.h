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

/**
 * @brief Setup NVS for use. An enum list of addresses should be defined in application code, with the maximum number of addresses being defined. 
 * This must be called before any other NVS functions. This mounts the NVS partition and initializes it. The nvs_address_count will be used to know 
 * when the address is out of bounds.
 * 
 * @param nvs_address_count the number of addresses to use in the NVS partition
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_setup(uint8_t nvs_address_count);

/**
 * @brief Write given data buffer to NVS with given address.
 * 
 * @param address address of NVS to write to
 * @param data pointer to data buffer to write
 * @param length length of data to write
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_write(uint8_t address, const void *data, size_t length);

/**
 * @brief Read data from NVS with given address, data is written to the provided buffer.
 * 
 * @param address address of NVS to read from
 * @param data pointer to data buffer to write
 * @param length length of data to read
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_read(uint8_t address, void *data, size_t length);

/**
 * @brief Delete data from NVS with given address.
 * 
 * @param address address of NVS to delete
 * @return int 0 on success, -1 on error
 */
int sensor_nvs_delete(uint8_t address);

/**
 * @brief Clear all data from NVS. NVS must be setup again after clearing.
 * 
 * @return int 0 on success, -ERRNO errno code if error
 */
int sensor_nvs_clear(void);

#endif