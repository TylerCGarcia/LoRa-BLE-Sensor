/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 */

#include <zephyr/ztest.h>
#include <zephyr/drivers/counter.h>

ZTEST_SUITE(timer, NULL, NULL, NULL, NULL, NULL);

ZTEST(timer, test_timer_setup)
{
    zassert_ok(0,"TEST");
}