/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_power.h"
#include "sensor_data.h"
#include <zephyr/drivers/adc.h>

LOG_MODULE_REGISTER(MAIN);

sensor_power_config_t sensor_output1 = {
	.power_id = SENSOR_POWER_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1),
	.delay_ms = 100
};

sensor_power_config_t sensor_output2 = {
	.power_id = SENSOR_POWER_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2),
	.delay_ms = 100
};

sensor_data_config_t sensor1_data_config = {
    .id = SENSOR_1,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d1), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d2), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor1),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor1)
};

sensor_data_config_t sensor2_data_config = {
    .id = SENSOR_2,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor2d1), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor2d2), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor2),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor2)
};

static void setup_power(void)
{
	sensor_power_init(&sensor_output1);
	sensor_power_init(&sensor_output2);
}

static void set_and_validate_output(sensor_power_config_t *sensor, enum sensor_voltage index, int accepted_error)
{
	int ret;
	char voltage_name[20];
	if (index == SENSOR_VOLTAGE_OFF)
	{
		k_msleep(1000);
	}
	get_sensor_voltage_name(voltage_name, index);
	LOG_INF("Setting Output %s", voltage_name);
	set_sensor_output(sensor, index);
	LOG_INF("READ VOLTAGE: %f", read_sensor_output(sensor));
	ret = validate_output(sensor, index, accepted_error);
	LOG_INF("OUTPUT is %s", ret ? "INVALID":"VALID");

}

static void setup_sensor_data(void)
{
	sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
	sensor_data_setup(&sensor2_data_config, VOLTAGE_SENSOR);
}

int main(void)
{
	int accepted_error = 5;
	setup_power();
	setup_sensor_data();
	int i = 0;
	while (1) 
	{
		set_and_validate_output(&sensor_output1, SENSOR_VOLTAGE_12V, accepted_error);
		set_and_validate_output(&sensor_output2, SENSOR_VOLTAGE_12V, accepted_error);
		float voltage1 = get_sensor_voltage_reading(&sensor1_data_config);
		LOG_INF("VOLTAGE1: %f", voltage1);
		float voltage2 = get_sensor_voltage_reading(&sensor2_data_config);
		LOG_INF("VOLTAGE2: %f", voltage2);
		k_msleep(1000);
		i = (i+1)%SENSOR_VOLTAGE_INDEX_LIMIT;
	}
	return 0;
}
