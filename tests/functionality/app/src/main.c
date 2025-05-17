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
#include "sensor_app.h"

#include <zephyr/logging/log.h>
#include <zephyr/fff.h>
#include "sensor_ble_fakes.h"
#include "sensor_power_fakes.h"
#include "sensor_reading_fakes.h"
#include "sensor_lorawan_fakes.h"

LOG_MODULE_REGISTER(tests_app, LOG_LEVEL_DBG);

DEFINE_FFF_GLOBALS;

/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{
    int ret;
    ret = sensor_nvs_clear();
    zassert_ok(ret, "Failed to clear NVS");
}

ZTEST_SUITE(app, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_init)
{
    int ret;
    ret = sensor_app_init();
    zassert_ok(ret, "App init failed");
    ret = sensor_nvs_clear();
    zassert_ok(ret, "Failed to clear NVS");
}

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_ble_init)
{
    int ret;
    ret = sensor_app_ble_start();
    zassert_ok(ret, "BLE init failed");
    k_sleep(K_SECONDS(1)); // Wait for the BLE thread to start
    zassert_equal(ble_setup_fake.call_count, 1, "BLE setup should be called");
}
