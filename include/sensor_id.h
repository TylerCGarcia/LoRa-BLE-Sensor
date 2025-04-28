/**
 * @file sensor_id.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor ids and sensor power ids.
 * This is to keep track of how many power configurations or sensor configurations a 
 * specific board may have.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_ID_H
#define SENSOR_ID_H

/**
 * @brief Sensor power identifiers
 */
typedef enum sensor_power_id {
    SENSOR_POWER_1 = 0,
    SENSOR_POWER_2,
    SENSOR_POWER_INDEX_LIMIT // This is used to get the size of the enum
};

/**
 * @brief Sensor identifiers
 */
typedef enum sensor_id {
    SENSOR_1 = 0,
    SENSOR_2,
    SENSOR_INDEX_LIMIT // This is used to get the size of the enum
};

#endif