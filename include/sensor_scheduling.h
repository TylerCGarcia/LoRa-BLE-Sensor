/**
 * @file sensor_scheduling.h
 * @author Tyler Garcia
 * @brief This is a library to schedule sensor readings and radio transmissions.
 * @version 0.1
 * @date 2025-05-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_SCHEDULING_H
#define SENSOR_SCHEDULING_H

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

enum sensor_scheduling_id{
    SENSOR_SCHEDULING_ID_RADIO,
    SENSOR_SCHEDULING_ID_SENSOR1,
    SENSOR_SCHEDULING_ID_SENSOR2,
    SENSOR_SCHEDULING_ID_LIMIT,
};

typedef struct {
    /* The id of the component to schedule */
    enum sensor_scheduling_id id;
    // void (*callback)(void);        // Callback function for the scheduled event
    uint32_t frequency_seconds;       // Frequency between events in seconds
    /* Whether this component is currently scheduled */
    uint8_t is_scheduled;
    /* Whether this component schedule has been triggered */
    uint8_t is_triggered;
    /* Last event time in seconds */
    uint32_t last_event_time;

} sensor_scheduling_cfg_t;

int sensor_scheduling_init(const struct device *timer);

int sensor_scheduling_add_schedule(sensor_scheduling_cfg_t *schedule);

int sensor_scheduling_remove_schedule(sensor_scheduling_cfg_t *schedule);

#endif