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

LOG_MODULE_REGISTER(tests_timer, LOG_LEVEL_DBG);

const struct device *timer0 = DEVICE_DT_GET(DT_NODELABEL(counter0));
// const struct device *timer1 = DEVICE_DT_GET(DT_NODELABEL(counter1));

ZTEST_SUITE(timer, NULL, NULL, NULL, NULL, NULL);

ZTEST(timer, test_timer_init)
{
    int ret;   
    ret = sensor_timer_init(timer0);
    zassert_ok(ret,"Sensor timer init failed");
}

ZTEST(timer, test_timer_get_time)
{
    int ret;
    ret = sensor_timer_init(timer0);
    zassert_ok(ret,"Sensor timer init failed");
    LOG_INF("Timer Frequency: %d", counter_get_frequency(timer0));
    k_sleep(K_SECONDS(1));
    uint32_t time = sensor_timer_get_seconds(timer0);
    zassert_true(time > 0 && time < 2, "Timer time %d", time);
}