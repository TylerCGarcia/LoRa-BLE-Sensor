/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MAIN);

int main(void)
{

	while (1) 
	{
		LOG_INF("Hello World");
		k_sleep(K_SECONDS(1));
	}
	return 0;
}
