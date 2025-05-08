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

/**
 * @brief Enum of addresses to use in the NVS partition.
 */
enum sensor_nvs_address {
    SENSOR_NVS_ADDRESS_0,
    SENSOR_NVS_ADDRESS_1,
    SENSOR_NVS_ADDRESS_2,
    SENSOR_NVS_ADDRESS_3,
    SENSOR_NVS_ADDRESS_4,
    SENSOR_NVS_ADDRESS_5,
    SENSOR_NVS_ADDRESS_6,
    SENSOR_NVS_ADDRESS_7,
    SENSOR_NVS_ADDRESS_8,
    SENSOR_NVS_ADDRESS_9,
    SENSOR_NVS_ADDRESS_LIMIT
};

/**
 * @brief Setup sensor power systems 
 * 
 */
// static void *testsuite_setup(void)
// {
//     int ret;
//     ret = sensor_nvs_setup();
//     zassert_ok(ret, "Failed to initialize NVS");
// }


/**
 * @brief Setup sensor power systems 
 * 
 */
static void *before_tests(void)
{
    int ret;
    ret = sensor_nvs_setup(SENSOR_NVS_ADDRESS_LIMIT);
    zassert_ok(ret, "Failed to initialize NVS");
}

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

/**
 * @brief Test Sensor NVS
 *
 * This test suite verifies the Sensor NVS system is working correctly.
 *
 */
ZTEST_SUITE(nvs, NULL, NULL, before_tests, after_tests, NULL);

/**
 * @brief Confirm data can be written to NVS
 * 
 */
ZTEST(nvs, test_sensor_nvs_write)
{
    int ret;
    static uint8_t test_data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_0, test_data, sizeof(test_data));
    zassert_ok(ret, "Failed to write to NVS");
}

/**
 * @brief Confirm data can't be written to NVS with an out of bounds data length
 * 
 */
ZTEST(nvs, test_sensor_nvs_write_length_out_of_bounds)
{
    static uint8_t test_data_out_of_bounds[SENSOR_NVS_MAX_SIZE+1] = {0};
    int ret;
    ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_1, test_data_out_of_bounds, sizeof(test_data_out_of_bounds));
    zassert_not_ok(ret, "Failed to write to NVS");
}

/**
 * @brief Confirm data can be read from NVS
 * 
 */
ZTEST(nvs, test_sensor_nvs_read)
{
    int ret;
    uint8_t test_data[SENSOR_NVS_MAX_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
    ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_2, test_data, sizeof(test_data));
    static uint8_t test_data_read[SENSOR_NVS_MAX_SIZE] = {0};
    ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_2, test_data_read, sizeof(test_data_read));
    zassert_ok(ret, "Failed to read from NVS");
    zassert_mem_equal(test_data, test_data_read, sizeof(test_data), "Data read from NVS does not match data written to NVS");
}

/**
 * @brief Confirm data can't be read from NVS with an out of bounds data length
 * 
 */
ZTEST(nvs, test_sensor_nvs_read_length_out_of_bounds)
{
    int ret;
    static uint8_t test_data_read[SENSOR_NVS_MAX_SIZE+1] = {0};
    ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_2, test_data_read, sizeof(test_data_read));
    zassert_not_ok(ret, "Failed to read from NVS");
}

/**
 * @brief Confirm data can be deleted from NVS
 * 
 */
ZTEST(nvs, test_sensor_nvs_delete)
{
    int ret;
    static uint8_t test_data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_2, test_data, sizeof(test_data));
    zassert_ok(ret, "Failed to write to NVS");
    static uint8_t test_data_read[16] = {0};
    ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_2, test_data_read, sizeof(test_data_read));
    zassert_mem_equal(test_data, test_data_read, sizeof(test_data), "Data read from NVS does not match data written to NVS");
    ret = sensor_nvs_delete(SENSOR_NVS_ADDRESS_2);
    zassert_ok(ret, "Failed to delete from NVS");
    ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_2, test_data_read, sizeof(test_data_read));
    zassert_not_ok(ret, "Should fail to read from NVS after deletion");
}

/**
 * @brief Confirm data can't be written to NVS with an out of bounds address
 * 
 */
ZTEST(nvs, test_sensor_nvs_write_index_out_of_bounds)
{
    int ret;
    uint8_t test_data[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_LIMIT, test_data, sizeof(test_data));
    zassert_not_ok(ret, "Should fail to write to NVS out of bounds");
}

/**
 * @brief Confirm data can't be read from NVS with an out of bounds address
 * 
 */
ZTEST(nvs, test_sensor_nvs_read_index_out_of_bounds)
{
    int ret;
    static uint8_t test_data_read[SENSOR_NVS_MAX_SIZE] = {0};
    ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_LIMIT, test_data_read, sizeof(test_data_read));
    zassert_not_ok(ret, "Should fail to read from NVS out of bounds");
}   

/**
 * @brief Confirm data can't be deleted from NVS with an out of bounds address
 * 
 */
ZTEST(nvs, test_sensor_nvs_delete_index_out_of_bounds)
{
    int ret;
    ret = sensor_nvs_delete(SENSOR_NVS_ADDRESS_LIMIT);
    zassert_not_ok(ret, "Should fail to delete from NVS out of bounds");
}

