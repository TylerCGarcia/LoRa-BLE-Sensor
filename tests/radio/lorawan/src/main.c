/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 */

#include <zephyr/ztest.h>

#include "sensor_lorawan.h"

ZTEST_SUITE(lorawan, NULL, NULL, NULL, NULL, NULL);

ZTEST(lorawan, test_setup)
{
    zassert_ok(0,"TEST");
}