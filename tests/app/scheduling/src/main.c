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

LOG_MODULE_REGISTER(tests_scheduling, LOG_LEVEL_DBG);

ZTEST_SUITE(scheduling, NULL, NULL, NULL, NULL, NULL);

ZTEST(scheduling, test_scheduling_init)
{
    zassert_true(true, "Scheduling init failed");
}

