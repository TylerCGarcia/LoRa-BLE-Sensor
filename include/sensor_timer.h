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

#include <zephyr/drivers/counter.h>
#include <zephyr/device.h>

/**
 * @brief Structure to hold the sensor timer data.
 */
typedef struct {
	counter_alarm_callback_t callback;
    uint32_t alarm_seconds;
    uint8_t channel;
} sensor_timer_alarm_cfg_t;

#define MINUTES_TO_SECONDS(minutes) (minutes * 60)

/**
 * @brief Initialize the chosen sensor timer instance.
 * @param dev The timer device to initialize.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_start(const struct device *dev);

/**
 * @brief Get the seconds elapsed since the timer was started.
 * @param dev The timer device to get the seconds from.
 * @return The seconds elapsed since the timer was started, or negative error code on failure.
 */
int sensor_timer_get_seconds(const struct device *dev);

/**
 * @brief Stop the chosen sensor timer instance. Timer will remain at the current value.
 * @param dev The timer device to stop.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_stop(const struct device *dev);

/**
 * @brief Reset the chosen sensor timer instance.
 * @param dev The timer device to reset.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_reset(const struct device *dev);

/**
 * @brief Set the alarm for the chosen sensor timer instance, also setting the top value of the timer to the chosen alarm value. 
 * When the alarm is triggered, the callback function will be called and the timer will be reset.
 * @param dev The timer device to set the alarm for.
 * @param sensor_timer_alarm_cfg The alarm configuration to set.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_set_alarm(const struct device *dev, sensor_timer_alarm_cfg_t *sensor_timer_alarm_cfg);

/**
 * @brief Cancel the alarm for the chosen channel of the sensor timer instance.
 * @param dev The timer device to cancel the alarm for.
 * @param sensor_timer_alarm_cfg The alarm configuration to cancel.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_cancel_alarm(const struct device *dev, sensor_timer_alarm_cfg_t *sensor_timer_alarm_cfg);


#endif