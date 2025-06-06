/**
 * @file sensor_names.h
 * @author Tyler Garcia
 * @brief This is a library for sensor names, converting between strings and enum values to make configurations human readable.
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_NAMES_H
#define SENSOR_NAMES_H

#include "sensor_id.h"

/**
 * @brief Get the name for the selected sensor voltage
 * 
 * @param voltage_name char array for the name to be saved to
 * @param voltage enum sensor_voltage setting selected
 * @return int 0 if successful, -1 if invalid input
 */
int get_sensor_voltage_name_from_index(char * voltage_name, enum sensor_voltage voltage);

/**
 * @brief Get the index for the selected sensor voltage
 * 
 * @param voltage_name char array for the name to be saved to
 * @return int index of the selected sensor voltage
 */
int get_sensor_voltage_index_from_name(char * voltage_name);

#endif