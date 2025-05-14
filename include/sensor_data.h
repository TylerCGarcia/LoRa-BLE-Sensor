/**
 * @file sensor_data.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor data, using the sensor_reading library to collect 
 * data from the sensors. This library will also handle the data storage and retrieval.
 * @version 0.1
 * @date 2025-05-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "sensor_id.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Types of data that can be stored in the sensor data buffer.
 */
typedef enum sensor_data_type {
    DATA_TYPE_UINT8,
    DATA_TYPE_UINT16,
    DATA_TYPE_UINT32,
    DATA_TYPE_FLOAT,
    DATA_TYPE_LIMIT
};

typedef struct {
    /* Sensor id to use for the sensor. */
    enum sensor_id id;
    /* Power id to use for the sensor. */
    enum sensor_power_id power_id;
} sensor_data_t;

/**
 * @brief Setup the sensor data. If the sensor chosen has continuous power, the power will be turned on when the sensor is setup. If not 
 * the power will be turned on only when the data is read. PULSE_SENSORs use continuous power. 
 * 
 * @param sensor_data The sensor data to setup holding the sensor id and power id.
 * @param type The type of sensor to setup.
 * @param voltage_enum The voltage to use for the sensor.
 * @return int 0 if successful, -1 if failed.
 */
int sensor_data_setup(sensor_data_t *sensor_data, enum sensor_types type, enum sensor_voltage voltage_enum);

/**
 * @brief Read the sensor data, and store the data in the buffer. It adds a tinestamp to the data buffer.
 * 
 * @param sensor_data The sensor data to read.
 * @param timestamp The timestamp to add to the data buffer.
 * @return int 0 if successful, -1 if failed.
 */
int sensor_data_read(sensor_data_t *sensor_data, uint32_t timestamp);

// /**
//  * @brief Get the sensor data buffer.
//  * 
//  * @param sensor_data The sensor data to get the buffer from.
//  * @return sensor_data_buffer_t* The buffer.
//  */
// sensor_data_buffer_t *sensor_data_get_buffer(sensor_data_t *sensor_data);

#endif