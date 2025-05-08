/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * What is stored in the NVS?
 * - Sensor Name (string)
 * - If device is configured (bool)
 * - LoRaWAN dev_eui (uint8_t[8])
 * - LoRaWAN app_eui (uint8_t[8])
 * - LoRaWAN app_key (uint8_t[16])
 * - Sensor power configuration (enum)
 * - Sensor configuration (enum)
 * 
 * Tests:
 */

#include <zephyr/ztest.h>
#include "sensor_nvs.h"

static uint8_t test_data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

/**
 * @brief Setup sensor power systems 
 * 
 */
// static void *testsuite_setup(void)
// {
//     int ret;
//     ret = nvs_setup();
//     zassert_ok(ret, "Failed to initialize NVS");
// }

ZTEST_SUITE(nvs, NULL, NULL, NULL, NULL, NULL);

ZTEST(nvs, test_nvs_setup)
{
    int ret;
    ret = nvs_setup();
    zassert_ok(ret, "Failed to initialize NVS");
}

ZTEST(nvs, test_nvs_write)
{
    int ret;
    ret = nvs_write();
    zassert_ok(ret, "Failed to write to NVS");
}

