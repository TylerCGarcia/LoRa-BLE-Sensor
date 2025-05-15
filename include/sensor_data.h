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
#include <zephyr/sys/ring_buffer.h>

#define MAX_DATA_BUFFER_SIZE 100

/**
 * @brief Types of data that can be stored in the sensor data buffer. Currently unused, plans to keep it
 * for future use to potentially store in sensor_data_t struct.
 */
typedef enum sensor_data_type {
    DATA_TYPE_INT,
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
    /* Ring buffer for sensor data. */
    struct ring_buf data_ring_buf;
    /* Ring buffer for timestamps. */
    struct ring_buf timestamp_ring_buf;
    /* Maximum number of samples to store in the buffer. */
    size_t max_samples;
    /* Size of the data in the buffer. */
    size_t data_size;
    /* Size of the timestamp in the buffer. */
    size_t timestamp_size;
    /* Pointer to latest data. */
    void *latest_data;
    /* Latest timestamp. */
    uint32_t latest_timestamp;
    /* Number of samples in the buffer. */
    uint32_t num_samples;
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

/**
 * @brief Print the sensor data from data and timestamp ring buffers. Displaying it from the oldest to newest.
 * 
 * @param sensor_data The sensor data to print.
 */
int sensor_data_print_data(sensor_data_t *sensor_data);

// /**
//  * @brief Get the latest reading from the sensor data.
//  * 
//  * @param sensor_data The sensor data to get the latest reading from.
//  * @param value The value to store the latest reading in.
//  * @param timestamp The timestamp to store the latest reading in.
//  */
// int sensor_data_get_latest_reading(sensor_data_t *sensor_data, void *value, uint32_t *timestamp);

/**
 * @brief Clear the sensor data.
 * 
 * @param sensor_data The sensor data to clear.
 */
int sensor_data_clear(sensor_data_t *sensor_data);

#endif