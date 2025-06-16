/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - test led can be turned on
 * - test led can be turned off
 */


#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/logging/log.h>
#include "sensor_pmic.h"
#include "sensor_led_fakes.h"

LOG_MODULE_REGISTER(tests_pmic, LOG_LEVEL_INF);

DEFINE_FFF_GLOBALS;

ZTEST_SUITE(pmic, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test that the PMIC can be initialized
 * 
 */
ZTEST(pmic, test_pmic_init)
{
    pmic_device_is_ready_fake.return_val = true;

    int ret;
    ret = sensor_pmic_init();
    zassert_ok(ret, "Failed to initialize PMIC");
}

// ZTEST(pmic, test_led_on)
// {
//     pmic_device_is_ready_fake.return_val = true;

//     int ret;
//     ret = sensor_pmic_init();
//     zassert_ok(ret, "Failed to initialize PMIC");
//     led_on_fake.return_val = 0;
//     ret = sensor_pmic_led_on();
//     zassert_ok(ret, "Failed to turn on LED");
// }