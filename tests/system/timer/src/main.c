/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Cancel all alarms
 * - Add to total seconds way to incorporate top value remainder, since topvalueseconds is not a whole number
 */

#include <zephyr/ztest.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <sensor_timer.h>
#include <zephyr/logging/log.h>
#include <errno.h>


LOG_MODULE_REGISTER(tests_timer, LOG_LEVEL_DBG);

static void alarm_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks);

enum sensor_timer_channel {
    SENSOR_TIMER_CHANNEL_0,
    SENSOR_TIMER_CHANNEL_1,
    SENSOR_TIMER_CHANNEL_2,
    SENSOR_TIMER_CHANNEL_LIMIT,
};

const struct device *timer0 = DEVICE_DT_GET(DT_NODELABEL(counter0));

static const int top_value_seconds = 3600;
static int counter_top_value_ticks;

static int channel_0_alarm_triggered = 0;
static int channel_1_alarm_triggered = 0;
static int channel_2_alarm_triggered = 0;

static sensor_timer_alarm_cfg_t alarm_cfg[SENSOR_TIMER_CHANNEL_LIMIT] = {
    [SENSOR_TIMER_CHANNEL_0].callback = alarm_callback,
    [SENSOR_TIMER_CHANNEL_0].alarm_seconds = 100,
    [SENSOR_TIMER_CHANNEL_0].channel = SENSOR_TIMER_CHANNEL_0,
    [SENSOR_TIMER_CHANNEL_0].is_alarm_set = 0,

    [SENSOR_TIMER_CHANNEL_1].callback = alarm_callback,
    [SENSOR_TIMER_CHANNEL_1].alarm_seconds = 100,
    [SENSOR_TIMER_CHANNEL_1].channel = SENSOR_TIMER_CHANNEL_1,
    [SENSOR_TIMER_CHANNEL_1].is_alarm_set = 0,

    [SENSOR_TIMER_CHANNEL_2].callback = alarm_callback,
    [SENSOR_TIMER_CHANNEL_2].alarm_seconds = 100,
    [SENSOR_TIMER_CHANNEL_2].channel = SENSOR_TIMER_CHANNEL_2,
    [SENSOR_TIMER_CHANNEL_2].is_alarm_set = 0,
};

/**
 * @brief Alarm callback function
 * 
 * @param dev 
 * @param chan_id 
 * @param ticks 
 */
static void alarm_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Alarm callback called");
    if (chan_id == SENSOR_TIMER_CHANNEL_0) {
        alarm_cfg[SENSOR_TIMER_CHANNEL_0].is_alarm_set = 0;
        channel_0_alarm_triggered = 1;
    } else if (chan_id == SENSOR_TIMER_CHANNEL_1) {
        alarm_cfg[SENSOR_TIMER_CHANNEL_1].is_alarm_set = 0;
        channel_1_alarm_triggered = 1;
    } else if (chan_id == SENSOR_TIMER_CHANNEL_2) {
        alarm_cfg[SENSOR_TIMER_CHANNEL_2].is_alarm_set = 0;
        channel_2_alarm_triggered = 1;
    }
}

/**
 * @brief Reset alarm triggered flags
 * 
 */
static void reset_alarm_triggered_flags(void)
{
    channel_0_alarm_triggered = 0;
    channel_1_alarm_triggered = 0;
    channel_2_alarm_triggered = 0;
}

/**
 * @brief Assert alarm triggered flags
 * 
 * @param channel0_expected_value 
 * @param channel1_expected_value 
 * @param channel2_expected_value 
 */
static void assert_alarm_triggered_flags(int channel0_expected_value, int channel1_expected_value, int channel2_expected_value)
{
    zassert_equal(channel_0_alarm_triggered, channel0_expected_value, "Alarm_0 expected value %d, actual value %d", channel0_expected_value, channel_0_alarm_triggered);
    zassert_equal(channel_1_alarm_triggered, channel1_expected_value, "Alarm_1 expected value %d, actual value %d", channel1_expected_value, channel_1_alarm_triggered);
    zassert_equal(channel_2_alarm_triggered, channel2_expected_value, "Alarm_2 expected value %d, actual value %d", channel2_expected_value, channel_2_alarm_triggered);
}

static void *testsuite_setup(void)
{
    int ret;
    counter_top_value_ticks = top_value_seconds / counter_get_frequency(timer0);
    /* Set the top value of the timer */
    const struct counter_top_cfg top_cfg = {
        .ticks = counter_top_value_ticks,
    };
    counter_set_top_value(timer0, &top_cfg);
    ret = sensor_timer_init(timer0);
    zassert_ok(ret, "Failed to initialize sensor timer");
}

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *before_tests(void)
{
    int ret;   
    ret = sensor_timer_start(timer0);
    zassert_ok(ret,"Sensor timer init failed");
}

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{
    int ret;
    if (alarm_cfg[SENSOR_TIMER_CHANNEL_0].is_alarm_set) {
        sensor_timer_cancel_alarm(timer0, SENSOR_TIMER_CHANNEL_0);
    }
    if (alarm_cfg[SENSOR_TIMER_CHANNEL_1].is_alarm_set) {
        sensor_timer_cancel_alarm(timer0, SENSOR_TIMER_CHANNEL_1);
    }
    if (alarm_cfg[SENSOR_TIMER_CHANNEL_2].is_alarm_set) {
        sensor_timer_cancel_alarm(timer0, SENSOR_TIMER_CHANNEL_2);
    }
    ret = sensor_timer_stop(timer0);
    zassert_ok(ret, "Failed to clear NVS");
    ret = sensor_timer_reset(timer0);
    zassert_ok(ret, "Failed to clear NVS");
}

ZTEST_SUITE(timer, NULL, testsuite_setup, before_tests, after_tests, NULL);

/**
 * @brief Confirm timer get seconds works
 * 
 */
ZTEST(timer, test_timer_get_current_seconds)
{
    int ret;
    LOG_INF("Timer Frequency: %d", counter_get_frequency(timer0));
    k_sleep(K_SECONDS(1));
    int time = sensor_timer_get_current_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
}

/**
 * @brief Confirm timer reset returns value to zero
 * 
 */
ZTEST(timer, test_timer_reset_returns_value_to_zero)
{
    int ret;
    k_sleep(K_SECONDS(1));
    ret = sensor_timer_reset(timer0);
    zassert_ok(ret,"Sensor timer reset failed");
    int time = sensor_timer_get_current_seconds(timer0);
    zassert_true(time == 0, "Timer time %d", time);
}

/**
 * @brief Confirm timer get seconds doesn't change when timer is stopped
 * 
 */
ZTEST(timer, test_timer_get_current_seconds_doesn_t_change_when_timer_is_stopped)
{
    int ret;
    k_sleep(K_SECONDS(1));
    int time = sensor_timer_get_current_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
    ret = sensor_timer_stop(timer0);
    k_sleep(K_SECONDS(5));
    zassert_ok(ret, "Failed to stop timer");
    time = sensor_timer_get_current_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
}

/**
 * @brief Confirm alarm callback works 
 * 
 */
ZTEST(timer, test_timer_set_alarm_callback)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = 5;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(5));
    assert_alarm_triggered_flags(1, 0, 0);
}

/**
 * @brief Confirm alarm channel 0 is triggered when set after timer started for 5 minutes
 * 
 */
ZTEST(timer, test_timer_alarm_5_minutes)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = 300;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(1, 0, 0);
}

/**
 * @brief Confirm alarm channel 1 is triggered when set after timer started for 5 minutes
 * 
 */
ZTEST(timer, test_timer_alarm_5_minutes_channel_1)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_1].alarm_seconds = 300;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_1]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 1, 0);
}   

/**
 * @brief Confirm alarm channel 2 is triggered when set after timer started for 5 minutes
 * 
 */
ZTEST(timer, test_timer_alarm_5_minutes_channel_2)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_2].alarm_seconds = 300;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_2]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 0, 1);
}

/**
 * @brief Confirm alarm works when multiple alarms are set with the same callback
 * 
 */
ZTEST(timer, test_timer_alarm_channel_0_and_1_with_same_callback)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = 60;
    alarm_cfg[SENSOR_TIMER_CHANNEL_1].alarm_seconds = 300;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_1]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(1, 0, 0);
    reset_alarm_triggered_flags();
    k_sleep(K_MINUTES(5));
    assert_alarm_triggered_flags(0, 1, 0);
}

/**
 * @brief Confirm alarm is triggered triggers 1 minute after alarm is set and not when timer reaches 1 minute
 * 
 */
ZTEST(timer, test_timer_alarm_channel_0_when_set_after_timer_started_for_1_minute)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = 60;
    int ret;    
    k_sleep(K_MINUTES(1));
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    assert_alarm_triggered_flags(0, 0, 0);
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(1, 0, 0);
}

/**
 * @brief Confirm alarm is not triggered when cancelled
 * 
 */
ZTEST(timer, test_timer_not_triggered_when_alarm_is_cancelled)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = 60;
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_SECONDS(30));
    assert_alarm_triggered_flags(0, 0, 0);
    ret = sensor_timer_cancel_alarm(timer0, SENSOR_TIMER_CHANNEL_0);
    zassert_ok(ret, "Failed to cancel alarm");
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(0, 0, 0);
}

/**
 * @brief Confirm MINUTES_TO_SECONDS macro works
 * 
 */
ZTEST(timer, test_timer_alarm_minutes_to_seconds)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = MINUTES_TO_SECONDS(5);
    int ret;
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Failed to set alarm");
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(0, 0, 0);
    k_sleep(K_MINUTES(4));
    assert_alarm_triggered_flags(1, 0, 0);
}

/**
 * @brief Confirm alarm fails when channel is out of bounds
 * 
 */
ZTEST(timer, test_timer_alarm_out_of_bounds)
{
    // Reset the alarm triggered flag
    reset_alarm_triggered_flags();
    sensor_timer_alarm_cfg_t wrong_alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = 100,
        .channel = SENSOR_TIMER_CHANNEL_LIMIT,
    };
    int ret;
    ret = sensor_timer_set_alarm(timer0, &wrong_alarm_cfg);
    zassert_not_ok(ret, "Alarm should fail with out of bounds channel");
}


/**
 * @brief Confirm alarm fails when channel is out of bounds
 * 
 */
ZTEST(timer, test_timer_alarm_works_if_top_value_is_reached)
{
    reset_alarm_triggered_flags();
    alarm_cfg[SENSOR_TIMER_CHANNEL_0].alarm_seconds = MINUTES_TO_SECONDS(1);
    int ret;
    k_sleep(K_SECONDS(top_value_seconds - 30));
    ret = sensor_timer_set_alarm(timer0, &alarm_cfg[SENSOR_TIMER_CHANNEL_0]);
    zassert_ok(ret, "Alarm should work");
    assert_alarm_triggered_flags(0, 0, 0);
    k_sleep(K_MINUTES(1));
    assert_alarm_triggered_flags(1, 0, 0);
}

/**
 * @brief Confirm total seconds is correct after top value is reached
 * 
 */
ZTEST(timer, test_get_total_seconds_after_top_value_is_reached)
{
    int ret;
    int expected_total_seconds = top_value_seconds + 50;
    k_sleep(K_SECONDS(expected_total_seconds));
    uint32_t total_seconds = sensor_timer_get_total_seconds(timer0);
    zassert_true(total_seconds == expected_total_seconds, "Total seconds should be %d, actual value %d", expected_total_seconds, total_seconds);
}

/**
 * @brief Confirm total seconds is correct after top value is reached
 * 
 */
ZTEST(timer, test_get_total_seconds_after_top_value_is_reached_twice)
{
    int ret;
    int expected_total_seconds = (top_value_seconds * 2) + 50;
    k_sleep(K_SECONDS(expected_total_seconds));
    uint32_t total_seconds = sensor_timer_get_total_seconds(timer0);
    LOG_INF("Total seconds: %d with top value: %d", total_seconds, top_value_seconds);
    zassert_true(total_seconds == expected_total_seconds, "Total seconds should be %d, actual value %d", expected_total_seconds, total_seconds);
}

/**
 * @brief Confirm total seconds is correct before top value is reached
 * 
 */
ZTEST(timer, test_get_total_seconds_before_top_value_is_reached)
{
    int ret;
    int expected_total_seconds = (top_value_seconds - 50);
    k_sleep(K_SECONDS(expected_total_seconds));
    uint32_t total_seconds = sensor_timer_get_total_seconds(timer0);
    LOG_INF("Total seconds: %d with top value: %d", total_seconds, top_value_seconds);
    zassert_true(total_seconds == expected_total_seconds, "Total seconds should be %d, actual value %d", expected_total_seconds, total_seconds);
}
