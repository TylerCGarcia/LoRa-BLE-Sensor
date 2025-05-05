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
 * 
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
	bt_le_adv_stop();
	bt_disable();
}


ZTEST_SUITE(ble, NULL, NULL, NULL, after_tests, NULL);

ZTEST(ble, test_ble_adv_init)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	zassert_ok(ret, "BLE setup failed (err %d)\n", ret);
}

ZTEST(ble, test_ble_adv_name)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	zassert_ok(ret, "BLE setup failed (err %d)\n", ret);
	zassert_str_equal(bt_get_name(), ble_config.adv_name, "BLE name is %s when it should be %s", bt_get_name(), ble_config.adv_name);
}
