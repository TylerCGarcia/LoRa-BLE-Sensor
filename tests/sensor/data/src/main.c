/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Can be configured to analog or digital sensor
 * - can be configured to analog voltage or analog current 
 * - can be configured to digital sensor pulse 
 * - handles invalid sensor types 
 * - handles sensor timeouts
 */

#include <zephyr/ztest.h>

#include "sensor_data.h"

ZTEST_SUITE(data, NULL, NULL, NULL, NULL, NULL);

