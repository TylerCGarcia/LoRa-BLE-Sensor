/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>

#include "sensor_app.h"

LOG_MODULE_REGISTER(MAIN);

sensor_app_config_t sensor_app_config;

// static void init_lora_ble(void)
// {
// 	int ret;
// 	// ret = ble_setup(&ble_config);
// 	// ret = ble_lorawan_service_init(&setup);

// 	while(is_lorawan_configured(&setup) < 0)
// 	{
// 		ret = -1; // set to -1 to indicate that the lorawan is not configured
// 		LOG_ERR("LoRaWAN is not configured, waiting for configuration");
// 		k_sleep(K_SECONDS(1));
// 	}

// 	lorawan_log_network_config(&setup);
// 	ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
// 	if(ret < 0)
// 	{
// 		LOG_INF("No Stored Dev Nonce, using default and storing it in nvs");
// 		ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
// 		if(ret < 0)
// 		{
// 			LOG_ERR("Failed to store dev nonce in nvs");
// 		}
// 	}

// 	lorawan_setup(&setup);
// 	/* Store the dev nonce in nvs after finishing join attempts. */
// 	sensor_nvs_write(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
// }

// static int send_packet(void)
// {
// 	int ret;
// 	if(is_lorawan_connected())
// 	{
// 		LOG_INF("Sending LoRaWAN data");
// 		int i = 0;
// 		lorawan_data_t lorawan_data;
// 		uint8_t sensor1_data_buffer[(sensor1_data.data_size * sensor1_data.num_samples) + (sensor1_data.timestamp_size * sensor1_data.num_samples)];
// 		uint8_t sensor2_data_buffer[(sensor2_data.data_size * sensor2_data.num_samples) + (sensor2_data.timestamp_size * sensor2_data.num_samples)];
// 		uint8_t sensor1_data_buffer_len;
// 		uint8_t sensor2_data_buffer_len;
// 		sensor_data_format_for_lorawan(&sensor1_data, sensor1_data_buffer, &sensor1_data_buffer_len);
// 		sensor_data_format_for_lorawan(&sensor2_data, sensor2_data_buffer, &sensor2_data_buffer_len);

// 		for(int i = 0; i < sensor1_data_buffer_len; i++)
// 		{
// 			lorawan_data.data[i] = sensor1_data_buffer[i];
// 		}
// 		for(int i = 0; i < sensor2_data_buffer_len; i++)
// 		{
// 			lorawan_data.data[i + sensor1_data_buffer_len] = sensor2_data_buffer[i];
// 		}

// 		lorawan_data.length = sensor1_data_buffer_len + sensor2_data_buffer_len;
// 		lorawan_data.port = 2;
// 		lorawan_data.attempts = 10;
// 		lorawan_data.delay = 1000;
// 		ret = lorawan_send_data(&lorawan_data);
// 		if(ret < 0)
// 		{
// 			LOG_ERR("Failed to send packet");
// 			return -1;
// 		}
// 		return 0;
// 	}
// 	else
// 	{
// 		LOG_ERR("LoRaWAN is not connected");
// 		return -1;
// 	}
// }

// int sensor_app_running_state(void)
// {
//     int ret;
// 	sensor_scheduling_cfg_t radio_schedule = {
// 		.id = SENSOR_SCHEDULING_ID_RADIO,
// 		.frequency_seconds = MINUTES_TO_SECONDS(5)
// 	};

// 	sensor_scheduling_cfg_t sensor1_schedule = {
// 		.id = SENSOR_SCHEDULING_ID_SENSOR1,
// 		.frequency_seconds = MINUTES_TO_SECONDS(1)
// 	};

// 	sensor_scheduling_cfg_t sensor2_schedule = {
// 		.id = SENSOR_SCHEDULING_ID_SENSOR2,
// 		.frequency_seconds = MINUTES_TO_SECONDS(1)
// 	};
//     ret = sensor_scheduling_add_schedule(&sensor1_schedule);
// 	ret = sensor_scheduling_add_schedule(&sensor2_schedule);
// 	k_sleep(K_SECONDS(2));// Set radio schedule to seconds later to make sure the sensors are read first
// 	ret = sensor_scheduling_add_schedule(&radio_schedule);
//     ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
//     ret = sensor_data_setup(&sensor2_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);

// 	uint8_t sensor1_first_time_trigger = 1;
// 	uint8_t sensor2_first_time_trigger = 1;
// 	uint8_t radio_first_time_trigger = 1;
//     while (1) 
// 	{
// 		if(sensor1_schedule.is_triggered || sensor1_first_time_trigger)
// 		{
// 			sensor1_first_time_trigger = 0;
// 			LOG_INF("Sensor 1 schedule triggered");
// 			sensor_data_read(&sensor1_data, sensor_scheduling_get_seconds());
// 			sensor_data_print_data(&sensor1_data);
// 			sensor_scheduling_reset_schedule(&sensor1_schedule);
// 		}
// 		if(sensor2_schedule.is_triggered || sensor2_first_time_trigger)
// 		{
// 			sensor2_first_time_trigger = 0;
// 			LOG_INF("Sensor 2 schedule triggered");
// 			sensor_data_read(&sensor2_data, sensor_scheduling_get_seconds());
// 			sensor_data_print_data(&sensor2_data);
// 			sensor_scheduling_reset_schedule(&sensor2_schedule);
// 		}
// 		if(radio_schedule.is_triggered || radio_first_time_trigger)
// 		{
// 			radio_first_time_trigger = 0;
// 			LOG_INF("Radio schedule triggered");
// 			sensor_scheduling_reset_schedule(&radio_schedule);
			
// 			ret = send_packet();
// 			if(ret == 0)
// 			{
// 				sensor_data_clear(&sensor1_data);
// 				sensor_data_clear(&sensor2_data);
// 			}
// 			else
// 			{
// 				LOG_ERR("Failed to send packet");
// 			}
// 		}
// 		k_msleep(100);
// 	}
// }

int main(void)
{
	while (1) 
	{

	}
	return 0;
}
