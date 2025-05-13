/**
 * @file sensor_timer.h
 * @author Tyler Garcia
 * @brief This is a library to keep track of sensor and radio timers. Manages alarms for sensors and radio transmissions.
 * This library is used by the sensor_scheduling library to schedule sensor readings and radio transmissions.
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
    /* Callback function to call when the alarm is triggered */
    counter_alarm_callback_t callback;
    /* Alarm time in seconds */
    uint32_t alarm_seconds;
    /* Counter channel to set the alarm on */
    uint8_t channel;
    /* 0 = alarm not set, 1 = alarm set */
    uint8_t is_alarm_set;
} sensor_timer_alarm_cfg_t;

#define MINUTES_TO_SECONDS(minutes) (minutes * 60)

/**
 * @brief Initialize the chosen sensor timer instance and create a top value callback to increment the total overflown seconds.
 * @param dev The timer device to initialize.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_init(const struct device *dev);

/**
 * @brief Start the chosen sensor timer instance.
 * @param dev The timer device to start.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_start(const struct device *dev);

/**
 * @brief Get the current seconds the timer was started. This value resets every time the timer is reset or top value is reached.
 * @param dev The timer device to get the seconds from.
 * @return The seconds elapsed since the timer was started, or negative error code on failure.
 */
int sensor_timer_get_current_seconds(const struct device *dev);

/**
 * @brief Get the total seconds the timer has been running.
 * @param dev The timer device to get the total seconds from.
 * @return The total seconds the timer has been running, or negative error code on failure.
 */
int sensor_timer_get_total_seconds(const struct device *dev);

/**
 * @brief Stop the chosen sensor timer instance. Timer will remain at the current value.
 * @param dev The timer device to stop.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_stop(const struct device *dev);

/**
 * @brief Reset the chosen sensor timer instance. This also resets the total seconds to 0.
 * @param dev The timer device to reset.
 * @return 0 on success, negative error code on failure.
 */
int sensor_timer_reset(const struct device *dev);

/**
 * @brief Set the alarm for the chosen sensor timer instance, also setting the top value of the timer to the chosen alarm value. 
 * When the alarm is triggered, the callback function will be called and the timer will be reset. In alarm callback, the is_alarm_set needs to be set to 0 manually.
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