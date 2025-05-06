/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - connection callbacks
 * - ble advertisement interval
 * - bt_set_name
 * - ble advertising data
 * - ble advertising data update
 * - add in checks to know if the device is advertising
 */


#include <zephyr/ztest.h>

#include "sensor_ble.h"

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>

/**
 * @brief Call after each test
 * 
 */
static void *after_tests(void)
{
	int ret = ble_end();
	zassert_ok(ret, "BLE end failed (err %d)\n", ret);
}


ZTEST_SUITE(ble, NULL, NULL, NULL, after_tests, NULL);

/**
 * @brief Test the BLE advertisement initialization.
 * 
 */
ZTEST(ble, test_ble_adv_init)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	zassert_ok(ret, "BLE setup failed (err %d)\n", ret);
}

/**
 * @brief Test the BLE advertisement is advertising.
 * 
 */
ZTEST(ble, test_ble_adv_is_advertising)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	zassert_ok(ret, "BLE setup failed (err %d)\n", ret);
	zassert_true(is_ble_advertising(), "BLE is not advertising");
}

/**
 * @brief Test the BLE advertisement name change.
 * 
 */
ZTEST(ble, test_ble_adv_name_change)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	zassert_ok(ret, "BLE setup failed (err %d)\n", ret);
	memcpy(ble_config.adv_name, "BLE-LoRa-Sensor-2", strlen("BLE-LoRa-Sensor-2"));
	ble_change_name(&ble_config);
	zassert_str_equal(bt_get_name(), "BLE-LoRa-Sensor-2", "BLE name is %s when it should be %s", bt_get_name(), "BLE-LoRa-Sensor-2");
}

