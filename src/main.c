/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_power.h"

LOG_MODULE_REGISTER(MAIN);

static const char *sensor_output_name[] = {
	"SENSOR_VOLTAGE_OFF", 
	"SENSOR_VOLTAGE_3V3", 
	"SENSOR_VOLTAGE_5V",
	"SENSOR_VOLTAGE_6V",
	"SENSOR_VOLTAGE_12V",
	"SENSOR_VOLTAGE_24V"
};

sensor_power_config_t sensor_output1 = {
	.output_id = SENSOR_OUTPUT_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1))
};

sensor_power_config_t sensor_output2 = {
	.output_id = SENSOR_OUTPUT_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2))
};

static void sensor_setup(void)
{
	sensor_power_init(&sensor_output1);
	sensor_power_init(&sensor_output2);
}

int main(void)
{
	sensor_setup();
	int i = 0;
	while (1) 
	{
		LOG_INF("Setting Voltage %s", sensor_output_name[i]);
		set_sensor_voltage(&sensor_output1, i);
		k_sleep(K_SECONDS(10));
		i = (i+1)%SENSOR_VOLTAGE_INDEX_LIMIT;
		set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
		k_sleep(K_SECONDS(2));


	}
	return 0;
}
