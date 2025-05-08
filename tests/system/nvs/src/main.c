/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 */

#include <zephyr/ztest.h>


#define TEST_NVS_FLASH_AREA		        storage_partition
#define TEST_NVS_FLASH_AREA_OFFSET	    FIXED_PARTITION_OFFSET(TEST_NVS_FLASH_AREA)
#define TEST_NVS_FLASH_AREA_ID		    FIXED_PARTITION_ID(TEST_NVS_FLASH_AREA)
#define TEST_NVS_FLASH_AREA_DEV         DEVICE_DT_GET(DT_MTD_FROM_FIXED_PARTITION(DT_NODELABEL(TEST_NVS_FLASH_AREA)))
#define TEST_DATA_ID			        1
#define TEST_SECTOR_COUNT		        5U

ZTEST_SUITE(nvs, NULL, NULL, NULL, NULL, NULL);

ZTEST(nvs, test_nvs_setup)
{
    zassert_ok(0,"TEST");
}