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

enum sensor_timer_channel {
    SENSOR_TIMER_CHANNEL_0,
    SENSOR_TIMER_CHANNEL_1,
    SENSOR_TIMER_CHANNEL_2,
};

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

static int channel_0_alarm_triggered = 0;
static int channel_1_alarm_triggered = 0;
static int channel_2_alarm_triggered = 0;

static void reset_alarm_triggered_flags(void)
{
    channel_0_alarm_triggered = 0;
    channel_1_alarm_triggered = 0;
    channel_2_alarm_triggered = 0;
}

static void assert_alarm_triggered_flags(int channel0_expected_value, int channel1_expected_value, int channel2_expected_value)
{
    zassert_equal(channel_0_alarm_triggered, channel0_expected_value, "Alarm_0 expected value %d, actual value %d", channel0_expected_value, channel_0_alarm_triggered);
    zassert_equal(channel_1_alarm_triggered, channel1_expected_value, "Alarm_1 expected value %d, actual value %d", channel1_expected_value, channel_1_alarm_triggered);
    zassert_equal(channel_2_alarm_triggered, channel2_expected_value, "Alarm_2 expected value %d, actual value %d", channel2_expected_value, channel_2_alarm_triggered);
}

static void alarm_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Alarm callback called");
    if (chan_id == SENSOR_TIMER_CHANNEL_0) {
        channel_0_alarm_triggered = 1;
    } else if (chan_id == SENSOR_TIMER_CHANNEL_1) {
        channel_1_alarm_triggered = 1;
    } else if (chan_id == SENSOR_TIMER_CHANNEL_2) {
        channel_2_alarm_triggered = 1;
    }
}

ZTEST(timer, test_timer_set_alarm_callback)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 5,
        .channel = SENSOR_TIMER_CHANNEL_0, 
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(5));
    assert_alarm_triggered_flags(1, 0, 0);
}

ZTEST(timer, test_timer_alarm_5_minutes)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 300,
        .channel = SENSOR_TIMER_CHANNEL_0,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(1, 0, 0);
}

ZTEST(timer, test_timer_alarm_5_minutes_channel_1)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 300,
        .channel = SENSOR_TIMER_CHANNEL_1,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 1, 0);
}   

ZTEST(timer, test_timer_alarm_5_minutes_channel_2)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 300,
        .channel = SENSOR_TIMER_CHANNEL_2,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 0, 1);
}

ZTEST(timer, test_timer_alarm_channel_0_and_1_with_same_callback)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 60,
        .channel = SENSOR_TIMER_CHANNEL_0,
    };
    sensor_timer_alarm_cfg_t alarm_cfg_2 = {
        .callback = alarm_callback,
        .alarm_seconds = 300,
        .channel = SENSOR_TIMER_CHANNEL_1,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg_2);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(1, 0, 0);
    reset_alarm_triggered_flags();
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 1, 0);
}

ZTEST(timer, test_timer_alarm_channel_0_when_set_after_timer_started_for_1_minute)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 60,
        .channel = SENSOR_TIMER_CHANNEL_0,
    };
    int ret;    
    k_sleep(K_MINUTES(1));
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
    zassert_ok(ret, "Failed to set alarm");
    assert_alarm_triggered_flags(0, 0, 0);
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(1, 0, 0);
}

