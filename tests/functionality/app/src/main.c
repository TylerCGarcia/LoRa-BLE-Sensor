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

sensor_app_config_t sensor_app_config = {
    .state = SENSOR_APP_STATE_CONFIGURATION,
    .is_lorawan_enabled = 0,
    .is_sensor_1_enabled = 0,
    .is_sensor_2_enabled = 0,
    .sensor_1_type = NULL_SENSOR,
    .sensor_2_type = NULL_SENSOR,
    .sensor_1_voltage = SENSOR_VOLTAGE_OFF,
    .sensor_2_voltage = SENSOR_VOLTAGE_OFF,
    .sensor_1_frequency = 0,
    .sensor_2_frequency = 0,
};

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

ZTEST_SUITE(app, NULL, NULL, NULL, after_tests, NULL);

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_init)
{
    int ret;
    ret = sensor_app_init(&sensor_app_config);
    zassert_ok(ret, "App init failed");
}


/**
 * @brief Test that the sensor app can is initialized in the configuration state when no configuration is set
 * 
 */
ZTEST(app, test_app_get_state)
{
    int ret;
    ret = sensor_app_init(&sensor_app_config);
    zassert_ok(ret, "App init failed");
    /* Test that the app is in the configuration state after initialization. */
    zassert_equal(sensor_app_config.state, SENSOR_APP_STATE_CONFIGURATION, "App state is not configuration");
}

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_configuration_state)
{
    int ret;
    ret = sensor_app_init(&sensor_app_config);
    zassert_ok(ret, "App init failed");
    ret = sensor_app_configuration_state();
    zassert_ok(ret, "App configuration state failed");
}

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_running_state_fails_when_state_is_configuration)
{
    int ret;
    ret = sensor_app_init(&sensor_app_config);
    zassert_ok(ret, "App init failed");
    /* Set the app to the running state. */
    ret = sensor_app_running_state();
    zassert_not_ok(ret, "App running state should fail when state is configuration");
}

/**
 * @brief Test that the sensor app can be initialized
 * 
 */
ZTEST(app, test_app_running_state_fails_when_neither_sensor_is_enabled)
{
    int ret;
    ret = sensor_app_init(&sensor_app_config);
    zassert_ok(ret, "App init failed");
    sensor_app_config.state = SENSOR_APP_STATE_RUNNING;
    /* Set the app to the running state. */
    ret = sensor_app_running_state();
    zassert_not_ok(ret, "App running state should fail when neither sensor is enabled");
}

// /**
//  * @brief Test that the sensor app can be initialized
//  * 
//  */
// ZTEST(app, test_app_ble_init)
// {
//     int ret;
//     ret = sensor_app_ble_start();
//     zassert_ok(ret, "BLE init failed");
//     k_sleep(K_SECONDS(1)); // Wait for the BLE thread to start
//     zassert_equal(ble_setup_fake.call_count, 1, "BLE setup should be called");
// }

