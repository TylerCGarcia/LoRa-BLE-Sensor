/**
 * @file sensor_names.c
 * @author Tyler Garcia
 * @brief This is a library for sensor names, converting between strings and enum values to make configurations human readable.
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "sensor_names.h"

LOG_MODULE_REGISTER(SENSOR_NAMES, LOG_LEVEL_INF);

const char *sensor_voltage_names[] = {
    [SENSOR_VOLTAGE_OFF] = "SENSOR_VOLTAGE_OFF",
    [SENSOR_VOLTAGE_3V3] = "SENSOR_VOLTAGE_3V3",
    [SENSOR_VOLTAGE_5V]  = "SENSOR_VOLTAGE_5V",
    [SENSOR_VOLTAGE_6V]  = "SENSOR_VOLTAGE_6V",
    [SENSOR_VOLTAGE_12V] = "SENSOR_VOLTAGE_12V",
    [SENSOR_VOLTAGE_24V] = "SENSOR_VOLTAGE_24V",
};

const char *sensor_type_names[] = {
    [NULL_SENSOR]    = "NULL_SENSOR",
    [VOLTAGE_SENSOR] = "VOLTAGE_SENSOR",
    [CURRENT_SENSOR] = "CURRENT_SENSOR",
    [PULSE_SENSOR]   = "PULSE_SENSOR",
};

int get_sensor_voltage_name_from_index(char * voltage_name, enum sensor_voltage voltage)
{
    LOG_DBG("Getting sensor voltage name");
    if(voltage < 0 || voltage >= SENSOR_VOLTAGE_INDEX_LIMIT)
    {
        LOG_ERR("Invalid voltage");
        return -1;
    }
    strcpy(voltage_name, sensor_voltage_names[voltage]);
    return 0;
}

int get_sensor_voltage_index_from_name(char * voltage_name)
{
    LOG_DBG("Getting sensor voltage index from name %s", voltage_name);
    for(int i = 0; i < SENSOR_VOLTAGE_INDEX_LIMIT; i++)
    {
        if(strcmp(voltage_name, sensor_voltage_names[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

int get_sensor_type_name_from_index(char * sensor_type_name, enum sensor_types sensor_type)
{
    LOG_DBG("Getting sensor type name");
    if(sensor_type < 0 || sensor_type >= SENSOR_TYPE_LIMIT)
    {
        LOG_ERR("Invalid sensor type");
        return -1;
    }
    strcpy(sensor_type_name, sensor_type_names[sensor_type]);
    return 0;
}

int get_sensor_type_index_from_name(char * sensor_type_name)
{
    LOG_DBG("Getting sensor type index from name %s", sensor_type_name);
    for(int i = 0; i < SENSOR_TYPE_LIMIT; i++)
    {
        if(strcmp(sensor_type_name, sensor_type_names[i]) == 0)
        {
            return i;
        }
    }
    LOG_ERR("Invalid sensor type name");
    return -1;
}