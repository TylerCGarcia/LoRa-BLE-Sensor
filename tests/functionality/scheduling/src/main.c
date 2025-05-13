/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Confirm schedule can be initialized
 * - multiple actions can be tied to one alarm 
 * - actions can be scheduled to occur at different times
 */

#include <zephyr/ztest.h>
#include "sensor_scheduling.h"
#include "zephyr/logging/log.h"
#include <zephyr/kernel.h>
LOG_MODULE_REGISTER(tests_scheduling, LOG_LEVEL_DBG);

const struct device *timer0 = DEVICE_DT_GET(DT_NODELABEL(counter0));

static void *testsuite_setup(void)
{
    int ret = sensor_scheduling_init(timer0);
    zassert_ok(ret, "Scheduling init failed");
}

sensor_scheduling_cfg_t radio_schedule = {
    .id = SENSOR_SCHEDULING_ID_RADIO,
    .frequency_seconds = 10
};

sensor_scheduling_cfg_t sensor1_schedule = {
    .id = SENSOR_SCHEDULING_ID_SENSOR1,
    .frequency_seconds = 10
};

sensor_scheduling_cfg_t sensor2_schedule = {
    .id = SENSOR_SCHEDULING_ID_SENSOR2,
    .frequency_seconds = 10
};

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{
    int ret;
    if (radio_schedule.is_scheduled) {
        ret = sensor_scheduling_remove_schedule(&radio_schedule);
        zassert_ok(ret, "Scheduling remove schedule failed");
    }
    if (sensor1_schedule.is_scheduled) {
        ret = sensor_scheduling_remove_schedule(&sensor1_schedule);
        zassert_ok(ret, "Scheduling remove schedule failed");
    }
    if (sensor2_schedule.is_scheduled) {
        ret = sensor_scheduling_remove_schedule(&sensor2_schedule);
        zassert_ok(ret, "Scheduling remove schedule failed");
    }
}

ZTEST_SUITE(scheduling, NULL, testsuite_setup, NULL, NULL, NULL);

ZTEST(scheduling, test_scheduling_add_schedule)
{
    int ret = sensor_scheduling_add_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
}

ZTEST(scheduling, test_scheduling_is_scheduled)
{
    int ret = sensor_scheduling_add_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
    zassert_true(radio_schedule.is_scheduled, "Schedule is not scheduled");
}

ZTEST(scheduling, test_scheduling_remove_schedule)
{
    int ret = sensor_scheduling_add_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
    ret = sensor_scheduling_remove_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling remove schedule failed");
    zassert_false(radio_schedule.is_scheduled, "Schedule is scheduled");
}

ZTEST(scheduling, test_scheduling_is_triggered)
{
    int ret = sensor_scheduling_add_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
    zassert_false(radio_schedule.is_triggered, "Schedule is triggered");
    k_sleep(K_SECONDS(10));
    zassert_true(radio_schedule.is_triggered, "Schedule is not triggered");
}

ZTEST(scheduling, test_scheduling_is_triggered_multiple)
{
    int ret = sensor_scheduling_add_schedule(&radio_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
    ret = sensor_scheduling_add_schedule(&sensor1_schedule);
    zassert_ok(ret, "Scheduling add schedule failed");
    k_sleep(K_SECONDS(10));
    zassert_true(radio_schedule.is_triggered, "Schedule is not triggered");
    zassert_true(sensor1_schedule.is_triggered, "Schedule is not triggered");
    // ret = sensor_scheduling_reset_schedule(&sensor1_schedule);
}
