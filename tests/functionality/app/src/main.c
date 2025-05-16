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
#include <zephyr/fff.h>
#include "sensor_ble_fakes.h"

#include "sensor_app.h"

LOG_MODULE_REGISTER(tests_app, LOG_LEVEL_DBG);

DEFINE_FFF_GLOBALS;

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
    // zassert_equal(ble_setup_fake.arg1.adv_opt, BT_LE_ADV_OPT_CONNECTABLE, "BLE setup should be called with BT_LE_ADV_OPT_CONNECTABLE");
    // zassert_equal(ble_setup_fake.arg1.adv_interval_min_ms, 100, "BLE setup should be called with 100ms advertising interval");
    // zassert_equal(ble_setup_fake.arg1.adv_interval_max_ms, 100, "BLE setup should be called with 100ms advertising interval");
    // zassert_equal(ble_setup_fake.arg1.connection_callbacks, NULL, "BLE setup should be called with NULL connection callbacks");
    // zassert_equal(ble_setup_fake.arg1.add_service_uuid, 0, "BLE setup should be called with 0 to not add service UUID");
    // zassert_equal(ble_setup_fake.arg1.adv_name, "Sensor App", "BLE setup should be called with Sensor App as the advertising name");
    
    // zassert_equal(set_sensor_output_fake.arg1_history[1], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
}
