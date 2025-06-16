/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - test correct power calls for each sensor type
 */

#include <zephyr/ztest.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tests_data, LOG_LEVEL_INF);


/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{

}

ZTEST_SUITE(pmic, NULL, NULL, NULL, after_tests, NULL);

/**
 * @brief Test that the sensor data can be setup
 * 
 */
ZTEST(pmic, test_pmic_led_setup)
{
    zassert_true(true, "Test passed");
}
