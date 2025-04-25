/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_power.h"
#include <zephyr/drivers/adc.h>

LOG_MODULE_REGISTER(MAIN);

sensor_power_config_t sensor_output1 = {
	.output_id = SENSOR_OUTPUT_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1)
};

sensor_power_config_t sensor_output2 = {
	.output_id = SENSOR_OUTPUT_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2)
};

static void sensor_setup(void)
{
	sensor_power_init(&sensor_output1);
	sensor_power_init(&sensor_output2);
}

int main(void)
{
	int ret;
	int accepted_error = 5;
	sensor_setup();
	int i = 0;
	while (1) 
	{
		char voltage_name[20];
		get_sensor_voltage_name(voltage_name, i);
		LOG_INF("Setting Output %s", voltage_name);
		set_sensor_output(&sensor_output1, i);
		k_sleep(K_SECONDS(1));
		LOG_INF("READ VOLTAGE: %f", read_sensor_output(&sensor_output1));
		ret = validate_output(&sensor_output1, i, accepted_error);
		LOG_INF("OUTPUT is %s", ret ? "INVALID":"VALID");
		k_sleep(K_SECONDS(5));
		i = (i+1)%SENSOR_VOLTAGE_INDEX_LIMIT;
		set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
		k_sleep(K_SECONDS(2));
	}
	return 0;
}
