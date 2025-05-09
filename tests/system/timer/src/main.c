/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 */

#include <zephyr/ztest.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <sensor_timer.h>
#include <zephyr/logging/log.h>
#include <errno.h>

LOG_MODULE_REGISTER(tests_timer, LOG_LEVEL_DBG);

const struct device *timer0 = DEVICE_DT_GET(DT_NODELABEL(counter0));
// const struct device *timer1 = DEVICE_DT_GET(DT_NODELABEL(counter1));

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *before_tests(void)
{
    int ret;   
    ret = sensor_timer_init(timer0);
    zassert_ok(ret,"Sensor timer init failed");
}

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{
    int ret;
    ret = sensor_timer_stop(timer0);
    zassert_ok(ret, "Failed to clear NVS");
    ret = sensor_timer_reset(timer0);
    zassert_ok(ret, "Failed to clear NVS");
}

ZTEST_SUITE(timer, NULL, NULL, before_tests, after_tests, NULL);

ZTEST(timer, test_timer_get_seconds)
{
    int ret;
    LOG_INF("Timer Frequency: %d", counter_get_frequency(timer0));
    k_sleep(K_SECONDS(1));
    int time = sensor_timer_get_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
}

ZTEST(timer, test_timer_reset_returns_value_to_zero)
{
    int ret;
    k_sleep(K_SECONDS(1));
    ret = sensor_timer_reset(timer0);
    zassert_ok(ret,"Sensor timer reset failed");
    int time = sensor_timer_get_seconds(timer0);
    zassert_true(time == 0, "Timer time %d", time);
}

ZTEST(timer, test_timer_get_seconds_doesn_t_change_when_timer_is_stopped)
{
    int ret;
    k_sleep(K_SECONDS(1));
    int time = sensor_timer_get_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
    ret = sensor_timer_stop(timer0);
    k_sleep(K_SECONDS(5));
    zassert_ok(ret, "Failed to stop timer");
    time = sensor_timer_get_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
}

static int is_alarm_triggered = 0;

static void alarm_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Alarm callback called");
    is_alarm_triggered = 1;
    sensor_timer_reset(timer0);
}

ZTEST(timer, test_timer_set_alarm_callback)
{
    // Reset the alarm triggered flag
    is_alarm_triggered = 0;
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 5,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(5));
    zassert_equal(is_alarm_triggered, 1, "Alarm did not trigger");    
}

ZTEST(timer, test_timer_resets_when_alarm_is_triggered)
{
    // Reset the alarm triggered flag
    is_alarm_triggered = 0;
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 5,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(5));
    zassert_equal(is_alarm_triggered, 1, "Alarm did not trigger");
    k_sleep(K_SECONDS(1));
    zassert_equal(sensor_timer_get_seconds(timer0), 1, "Timer did not reset");
}

ZTEST(timer, test_timer_alarm_)
{
    // Reset the alarm triggered flag
    is_alarm_triggered = 0;
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 300,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(5));
    zassert_equal(is_alarm_triggered, 1, "Alarm did not trigger");
    k_sleep(K_SECONDS(1));
    zassert_equal(sensor_timer_get_seconds(timer0), 1, "Timer did not reset");
}