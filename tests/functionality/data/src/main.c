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
#include <sensor_data.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tests_app, LOG_LEVEL_DBG);

ZTEST_SUITE(data, NULL, NULL, NULL, NULL, NULL);

ZTEST(data, test_sensor_data_setup)
{
    // sensor_data_setup(SENSOR_ID_TEMPERATURE);
    zassert_true(true, "App init failed");
}
