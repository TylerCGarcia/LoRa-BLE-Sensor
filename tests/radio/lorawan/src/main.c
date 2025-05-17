/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - lorawan_setup_fails
 * - lorawan_setup can be called multiple times
 * - lorawan_setup_fails_only_first_try
 * - give out dev nonce requirement
 * - give way to end join attempts early
 */

#include <zephyr/ztest.h>
#include <zephyr/fff.h>

#include <stdint.h>
#include <string.h>

#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>
#include <zephyr/lorawan/emul.h>

#define DEV_EUI {0x00, 0x80, 0xE1, 0x15, 0x00, 0x56, 0x9E, 0x08}
#define JOIN_EUI {0x60, 0x81, 0xF9, 0x1D, 0xE0, 0x47, 0x30, 0xAB}
#define APP_KEY {0xE1, 0x0E, 0x13, 0x72, 0xD6, 0xA4, 0x19, 0x95, 0x0C, 0x88, 0x19, 0x41, 0x04, 0x0D, 0x58, 0x03}

static uint8_t test_dev_eui[8] = DEV_EUI;
static uint8_t test_join_eui[8] = JOIN_EUI;
static uint8_t test_app_key[16] = APP_KEY;

static lorawan_setup_t setup = {
    .lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
    .uplink_class = LORAWAN_CLASS_A,
    .downlink_callback = NULL,
    .join_attempts = 3,
    .dev_nonce = 0,
    .delay = 1000,
    .dev_eui = DEV_EUI,
    .join_eui = JOIN_EUI,
    .app_key = APP_KEY,
};

static bool last_downlink_called;
static uint8_t last_downlink_port;
static uint8_t last_downlink_len;
static uint8_t last_downlink_data[64];

/**
 * @brief Call after each test
 * 
 */
static void *after_tests(void)
{
	// Reset all sensors to correct values after each test
    memcpy(setup.app_key, test_app_key, sizeof(test_app_key));    
	memcpy(setup.dev_eui, test_dev_eui, sizeof(test_dev_eui));
	memcpy(setup.join_eui, test_join_eui, sizeof(test_join_eui));
    setup.uplink_class = LORAWAN_CLASS_A;
}


static void *testsuite_setup(void)
{
    int ret;
    ret = is_lorawan_connected();
    zassert_equal(ret, 0, "lorawan is not connected before setup");
}

ZTEST_SUITE(lorawan, NULL, testsuite_setup, NULL, after_tests, NULL);

/**
 * @brief Test lorawan_setup with correct parameters
 */
ZTEST(lorawan, test_setup)
{
	int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
}

/**
 * @brief Test lorawan_setup with correct parameters
 */
ZTEST(lorawan, test_setup_fails_when_invalid_uplink_class)
{
	int ret;
    setup.uplink_class = 5;
    ret = sensor_lorawan_setup(&setup);
    zassert_not_ok(ret, "lorawan_setup should fail");
}


/**
 * @brief Test is_lorawan_connected after lorawan_setup
 */
ZTEST(lorawan, test_is_lorawan_connected)
{
	int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
	ret = is_lorawan_connected();
	zassert_equal(ret, 1, "lorawan is not connected after setup");
}

/**
 * @brief Test lorawan_setup fails when no dev_eui
 */
ZTEST(lorawan, test_setup_fails_when_no_dev_eui)
{
    int ret;
    uint8_t dev_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(setup.dev_eui, dev_eui, sizeof(dev_eui));
    ret = sensor_lorawan_setup(&setup);
    zassert_not_ok(ret, "lorawan_setup should fail");
}

/**
 * @brief Test lorawan_setup fails when no join_eui
 */
ZTEST(lorawan, test_setup_fails_when_no_join_eui)
{
    int ret;
    uint8_t join_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(setup.join_eui, join_eui, sizeof(join_eui));
    ret = sensor_lorawan_setup(&setup);
    zassert_not_ok(ret, "lorawan_setup should fail");
}

/**
 * @brief Test lorawan_setup fails when no app_key
 */
ZTEST(lorawan, test_setup_fails_when_no_app_key)
{
    int ret;
    uint8_t app_key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(setup.app_key, app_key, sizeof(app_key));
    
    ret = sensor_lorawan_setup(&setup);
    zassert_not_ok(ret, "lorawan_setup should fail");
}

/**
 * @brief Test lorawan_setup fails when no dev_eui and join_eui
 */
ZTEST(lorawan, test_setup_fails_when_no_dev_eui_and_join_eui)
{
    int ret;
    uint8_t dev_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t join_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(setup.dev_eui, dev_eui, sizeof(dev_eui));
    memcpy(setup.join_eui, join_eui, sizeof(join_eui));
    ret = sensor_lorawan_setup(&setup);
    zassert_not_ok(ret, "lorawan_setup should fail");
}

/**
 * @brief Test lorawan_send_data with correct parameters
 */
ZTEST(lorawan, test_send_packet)
{
    int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
    int i = 0;
    lorawan_data_t lorawan_data;
    lorawan_data.data[i++] = "H";
    lorawan_data.data[i++] = "e";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "o";
    lorawan_data.length = i;
    lorawan_data.port = 2;
    lorawan_data.attempts = 0;
    ret = sensor_lorawan_send_data(&lorawan_data);
    zassert_ok(ret, "lorawan_send_data failed: %d", ret);
}

/**
 * @brief Test lorawan_send_data fails when no data
 */
ZTEST(lorawan, test_send_data_fails_when_no_data)
{
    int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
    lorawan_data_t lorawan_data;
    lorawan_data.data[0] = NULL;
    lorawan_data.length = 0;
    lorawan_data.port = 1;
    lorawan_data.attempts = 0;
    ret = sensor_lorawan_send_data(&lorawan_data);
    zassert_not_ok(ret, "lorawan_send_data should fail");
}

/**
 * @brief Test lorawan_send_data resets data after send
 */
ZTEST(lorawan, test_send_data_resets_data_afer_send)
{
    int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
    int i = 0;
    lorawan_data_t lorawan_data;
    lorawan_data.data[i++] = "H";
    lorawan_data.data[i++] = "e";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "o";
    lorawan_data.length = i;
    lorawan_data.port = 1;
    lorawan_data.attempts = 0;
    lorawan_data.delay = 1000;
    ret = sensor_lorawan_send_data(&lorawan_data);
    zassert_ok(ret, "lorawan_send_data failed: %d", ret);
    zassert_is_null(lorawan_data.data[0], "lorawan_data.data should be NULL");
    zassert_equal(lorawan_data.length, 0, "lorawan_data.length should be 0");
}

/**
 * @brief Test lorawan_send_data with confirmed message
 */
ZTEST(lorawan, test_send_data_confirmed)
{
    int ret;
    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);
    int i = 0;
    lorawan_data_t lorawan_data;
    lorawan_data.data[i++] = "H";
    lorawan_data.data[i++] = "e";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "l";
    lorawan_data.data[i++] = "o";
    lorawan_data.length = i;
    lorawan_data.port = 2;
    lorawan_data.attempts = 1;
    lorawan_data.delay = 1000;
    ret = sensor_lorawan_send_data(&lorawan_data);
    zassert_ok(ret, "lorawan_send_data failed: %d", ret);
}

/**
 * @brief Test downlink callback
 */
static void dl_callback(uint8_t port, bool data_pending, int16_t rssi, int8_t snr, uint8_t len, const uint8_t *data)
{
    last_downlink_called = true;
    last_downlink_port = port;
    last_downlink_len = len;
    memcpy(last_downlink_data, data, len);
}

/**
 * @brief Test if the downlink callback functions correctly
 */
ZTEST(lorawan, test_send_data_downlink_callback)
{
    int ret;
    setup.downlink_callback.cb = dl_callback;
    setup.downlink_callback.port = LW_RECV_PORT_ANY;

    ret = sensor_lorawan_setup(&setup);
    zassert_equal(ret, 0, "lorawan_setup failed: %d", ret);

    const uint8_t *payload = "Hello";
    uint8_t downlink_port = 1;
    lorawan_emul_send_downlink(downlink_port, true, 0, 0, strlen(payload), payload);
    // Assert
    zassert_true(last_downlink_called, "Downlink callback was not called");
    zassert_equal(last_downlink_port, downlink_port, "Wrong port");
    zassert_equal(last_downlink_len, strlen(payload), "Wrong length");
    zassert_mem_equal(last_downlink_data, payload, strlen(payload), "Wrong data");
}

/**
 * @brief Test if the lorawan is configured
 */
ZTEST(lorawan, test_is_lorawan_configured)
{
    int ret;
    ret = is_lorawan_configured(&setup);
    zassert_equal(ret, 0, "lorawan is not configured");
}   

/**
 * @brief Test if the lorawan is not configured
 */
ZTEST(lorawan, test_is_lorawan_configured_fails_when_no_dev_eui)
{
    int ret;
    uint8_t dev_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t join_eui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t app_key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(setup.dev_eui, dev_eui, sizeof(dev_eui));
    memcpy(setup.join_eui, join_eui, sizeof(join_eui));
    memcpy(setup.app_key, app_key, sizeof(app_key));
    ret = is_lorawan_configured(&setup);
    zassert_not_ok(ret, "lorawan is configured when it should not be");
}