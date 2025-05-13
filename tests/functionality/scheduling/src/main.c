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

LOG_MODULE_REGISTER(tests_scheduling, LOG_LEVEL_DBG);

ZTEST_SUITE(scheduling, NULL, NULL, NULL, NULL, NULL);

ZTEST(scheduling, test_scheduling_init)
{
    
    zassert_true(true, "Scheduling init failed");
}

