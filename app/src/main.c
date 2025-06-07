/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/linker/linker-defs.h>

#include "sensor_app.h"

LOG_MODULE_REGISTER(MAIN);

sensor_app_config_t sensor_app_config = {
	.state = SENSOR_APP_STATE_CONFIGURATION,
	.connect_network_during_configuration = 1,
	.is_sensor_1_enabled = 0,
	.sensor_1_voltage = SENSOR_VOLTAGE_OFF,
	.sensor_1_type = NULL_SENSOR,
	.sensor_1_type_name = "NULL_SENSOR",
	.sensor_1_frequency = 0,
	.is_sensor_2_enabled = 0,
	.sensor_2_voltage = SENSOR_VOLTAGE_OFF,
	.sensor_2_type = NULL_SENSOR,
	.sensor_2_type_name = "NULL_SENSOR",
	.sensor_2_frequency = 0,
};

int main(void)
{
	int ret;
	printk("Address of sample %p\n", (void *)__rom_region_start);
	LOG_INF("LoRa-BLE-Sensor");
	LOG_INF("Hardware Version: %d.%d", CONFIG_HW_VERSION_MAJOR, CONFIG_HW_VERSION_MINOR);
	LOG_INF("Firmware Version: %s", CONFIG_APP_VERSION);

	ret = sensor_app_init(&sensor_app_config);
	if(ret < 0)
	{
		LOG_ERR("Failed to initialize sensor app");
		sensor_app_config.state = SENSOR_APP_STATE_ERROR;
		return -1;
	}
	sensor_app_ble_start();

	while (1) 
	{
		switch(sensor_app_config.state)
		{
			case SENSOR_APP_STATE_CONFIGURATION:
				sensor_app_configuration_state();
				break;
			case SENSOR_APP_STATE_RUNNING:
				sensor_app_running_state();
				break;
			case SENSOR_APP_STATE_ERROR:
				sensor_app_error_state();
				break;
			default:
				LOG_ERR("Invalid state");
				break;
		}
		k_sleep(K_SECONDS(1));
	}
	return 0;
}
