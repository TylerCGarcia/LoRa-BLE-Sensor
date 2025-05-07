/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include "sensor_ble.h"
#include "sensor_lorawan.h"
#include "sensor_power.h"
#include "sensor_data.h"


LOG_MODULE_REGISTER(MAIN);

#define DEV_EUI {0x00, 0x80, 0xE1, 0x15, 0x00, 0x56, 0x9E, 0x08}
#define JOIN_EUI {0x60, 0x81, 0xF9, 0x1D, 0xE0, 0x47, 0x30, 0xAB}
#define APP_KEY {0xE1, 0x0E, 0x13, 0x72, 0xD6, 0xA4, 0x19, 0x95, 0x0C, 0x88, 0x19, 0x41, 0x04, 0x0D, 0x58, 0x03}

static lorawan_setup_t setup = {
    .lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
    .uplink_class = LORAWAN_CLASS_A,
    .downlink_callback = NULL,
    .join_attempts = 0,
    .dev_nonce = 100,
    .delay = 1000,
    .dev_eui = DEV_EUI,
    .join_eui = JOIN_EUI,
    .app_key = APP_KEY,
};

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
	int accepted_error = 5;
	sensor_data_setup(&sensor1_data_config, PULSE_SENSOR);
	sensor_data_setup(&sensor2_data_config, PULSE_SENSOR);
	set_and_validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, accepted_error);
	set_and_validate_output(&sensor_output2, SENSOR_VOLTAGE_3V3, accepted_error);
}

static void run_sensor_tests(void)
{
	float voltage1 = get_sensor_voltage_reading(&sensor1_data_config);
	LOG_INF("VOLTAGE1: %f", voltage1);
	float voltage2 = get_sensor_voltage_reading(&sensor2_data_config);
	LOG_INF("VOLTAGE2: %f", voltage2);
	float current1 = get_sensor_current_reading(&sensor1_data_config);
	LOG_INF("CURRENT1: %f", current1);
	float current2 = get_sensor_current_reading(&sensor2_data_config);
	LOG_INF("CURRENT2: %f", current2);
	int pulses1 = get_sensor_pulse_count(&sensor1_data_config);
	LOG_INF("PULSES1: %d", pulses1);
	int pulses2 = get_sensor_pulse_count(&sensor2_data_config);
	LOG_INF("PULSES2: %d", pulses2);
}

static void send_packet(void)
{
	// LOG_INF("Starting Application");
    // lorawan_setup(&setup);

	if(is_lorawan_connected())
	{
		LOG_INF("Sending LoRaWAN data");
		int i = 0;
		lorawan_data_t lorawan_data;
		lorawan_data.data[i++] = 255;
		lorawan_data.data[i++] = 255;
		lorawan_data.data[i++] = 255;
		lorawan_data.data[i++] = 255;
		lorawan_data.length = i;
		lorawan_data.port = 2;
		lorawan_data.attempts = 10;
		lorawan_data.delay = 1000;
		lorawan_send_data(&lorawan_data);
	}
}

int main(void)
{
	ble_config_t ble_config = {
		.adv_name = "BLE-LoRa-Sensor"
	};
	int ret;
	ret = ble_setup(&ble_config);
	while (1) 
	{
		k_sleep(K_MINUTES(1));
	}
	return 0;
}
