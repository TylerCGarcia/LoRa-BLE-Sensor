/**
 * @file sensor_id.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor ids and sensor power ids. As well as the configuration for sensors and sensor power.
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
 * @brief Sensor identifiers
 */
enum sensor_id {
    SENSOR_1 = 0,
    SENSOR_2,
    SENSOR_INDEX_LIMIT // This is used to get the size of the enum
};

/**
 * @brief Types of sensors that can be configured.
 */
enum sensor_types {
    /* No sensor configuration is set. */
    NULL_SENSOR = 0,
    /* Sensor is set to read from voltage_read pin. */
    VOLTAGE_SENSOR,
    /* Sensor is set to read from current_read pin. */
    CURRENT_SENSOR,
    /* Sensor is set to read low pulses from d1 pin. */
    PULSE_SENSOR,
    /* Used to denote size of enum. */
    SENSOR_TYPE_LIMIT
};

/**
 * @brief Sensor power identifiers
 */
enum sensor_power_id {
    SENSOR_POWER_1 = 0,
    SENSOR_POWER_2,
    SENSOR_POWER_INDEX_LIMIT // This is used to get the size of the enum
};

/**
 * @brief Voltage levels for sensor power
 */
enum sensor_voltage {
    SENSOR_VOLTAGE_OFF = 0,
    SENSOR_VOLTAGE_3V3,
    SENSOR_VOLTAGE_5V,
    SENSOR_VOLTAGE_6V,
    SENSOR_VOLTAGE_12V,
    SENSOR_VOLTAGE_24V,
    SENSOR_VOLTAGE_INDEX_LIMIT
};

#endif