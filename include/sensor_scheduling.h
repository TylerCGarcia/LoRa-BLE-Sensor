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

/* The configuration for a schedule */
typedef struct {
    /* The id of the component to schedule */
    enum sensor_scheduling_id id;
    /* The frequency of the schedule in seconds */
    uint32_t frequency_seconds;
    /* Whether this component is currently scheduled */
    uint8_t is_scheduled;
    /* Whether this component schedule has been triggered */
    uint8_t is_triggered;
    /* Last event time in seconds */
    uint32_t last_event_time;

} sensor_scheduling_cfg_t;

/**
 * @brief Initialize the sensor scheduling module
 * 
 * @param timer The timer device to use for scheduling
 * @return int 0 on success, negative error code on failure
 */
int sensor_scheduling_init(const struct device *timer);

/**
 * @brief Add a schedule to the sensor scheduling module
 * 
 * @param schedule The schedule to add
 * @return int 0 on success, negative error code on failure
 */
int sensor_scheduling_add_schedule(sensor_scheduling_cfg_t *schedule);

/**
 * @brief Remove a schedule from the sensor scheduling module
 * 
 * @param schedule The schedule to remove
 * @return int 0 on success, negative error code on failure
 */
int sensor_scheduling_remove_schedule(sensor_scheduling_cfg_t *schedule);

/**
 * @brief Reset a schedule in the sensor scheduling module. This will set the alarm to trigger at the given frequency from the last event time.
 * Triggering the next event based on the last event time keeps the schedule from drifting.
 * 
 * @param schedule The schedule to reset
 * @return int 0 on success, negative error code on failure
 */
int sensor_scheduling_reset_schedule(sensor_scheduling_cfg_t *schedule);

#endif