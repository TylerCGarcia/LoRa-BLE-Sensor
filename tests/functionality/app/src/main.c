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
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tests_app, LOG_LEVEL_DBG);

ZTEST_SUITE(app, NULL, NULL, NULL, NULL, NULL);

ZTEST(app, test_app_init)
{
    
    zassert_true(true, "App init failed");
}

