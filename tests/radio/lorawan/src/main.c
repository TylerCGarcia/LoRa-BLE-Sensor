/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 */

#include <zephyr/ztest.h>

#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>
#include <zephyr/lorawan/emul.h>

static lorawan_setup_t setup = {
    .lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
    .join_attempts = 3,
    .dev_eui = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .join_eui = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .app_key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

ZTEST_SUITE(lorawan, NULL, NULL, NULL, NULL, NULL);

ZTEST(lorawan, test_setup)
{
    // const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	// struct lorawan_join_config join_cfg = {0};
	// int ret;
	// zassert_true(device_is_ready(lora_dev), "LoRa device not ready");
	// ret = lorawan_start();
	// zassert_equal(ret, 0, "lorawan_start failed: %d", ret);
	// ret = lorawan_join(&join_cfg);
	// zassert_equal(ret, 0, "lorawan_join failed: %d", ret);

	int ret;
    ret = lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
}

ZTEST(lorawan, test_setup_fail)
{
    int ret;
    ret = lorawan_setup(&setup);
    zassert_equal(ret, -1, "lorawan_setup should fail");
}
