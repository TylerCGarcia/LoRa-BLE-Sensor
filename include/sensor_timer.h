/**
 * @file sensor_timer.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor and radio timers. Manages when last readings were
 * taken and when the last radio transmission was sent.
 * @version 0.1
 * @date 2025-05-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_TIMER_H
#define SENSOR_TIMER_H

#include <zephyr/device.h>

/**
 * @brief Structure to hold the sensor timer data.
 */
// typedef struct sensor_timer {
//     struct k_timer timer;
//     uint32_t last_reading;
//     uint32_t last_transmission;
// } sensor_timer_t;


/**
 * @brief Initialize the chosen sensor timer instance.
 */
int sensor_timer_init(const struct device *dev);

uint32_t sensor_timer_get_seconds(const struct device *dev);


#endif