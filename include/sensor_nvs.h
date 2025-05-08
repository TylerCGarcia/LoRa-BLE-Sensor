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

int sensor_nvs_setup(void);

#endif