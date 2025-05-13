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
#include "sensor_nvs.h"
#include "ble_lorawan_service.h"
#include "sensor_timer.h"

LOG_MODULE_REGISTER(MAIN);


const struct device *timer0 = DEVICE_DT_GET(DT_NODELABEL(rtc2));


enum sensor_nvs_address {
	SENSOR_NVS_ADDRESS_DEV_NONCE,
	SENSOR_NVS_ADDRESS_JOIN_EUI,
	SENSOR_NVS_ADDRESS_APP_KEY,
	SENSOR_NVS_ADDRESS_DEV_EUI,
	SENSOR_NVS_ADDRESS_LIMIT,
};

enum sensor_timer_channel {
    SENSOR_TIMER_CHANNEL_0,
    SENSOR_TIMER_CHANNEL_1,
    SENSOR_TIMER_CHANNEL_2,
};

#define DEV_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define JOIN_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define APP_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

static lorawan_setup_t setup = {
	.lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
	.uplink_class = LORAWAN_CLASS_A,
	.downlink_callback = NULL,
	.join_attempts = 0,
	.dev_nonce = 160,
	.delay = 1000,
	.dev_eui = DEV_EUI,
	.join_eui = JOIN_EUI,
	.app_key = APP_KEY,
};

static sensor_power_config_t sensor_output1 = {
	.power_id = SENSOR_POWER_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1),
	.delay_ms = 100
};

static sensor_power_config_t sensor_output2 = {
	.power_id = SENSOR_POWER_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2),
	.delay_ms = 100
};

static sensor_data_config_t sensor1_data_config = {
	.id = SENSOR_1,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d1), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d2), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor1),
	.current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor1)
};

static sensor_data_config_t sensor2_data_config = {
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

static void init_lora_ble(void)
{
	ble_config_t ble_config = {
		.adv_opt = BT_LE_ADV_OPT_CONN,
		.adv_name = "BLE-LoRa-Sensor",
		.adv_interval_min_ms = 500,
		.adv_interval_max_ms = 510
	};
	int ret;
	ret = ble_setup(&ble_config);
	ret = ble_lorawan_service_init(&setup);
	ret = sensor_nvs_setup(SENSOR_NVS_ADDRESS_LIMIT);
	sensor_nvs_read(SENSOR_NVS_ADDRESS_DEV_EUI, &setup.dev_eui, sizeof(setup.dev_eui));
	sensor_nvs_read(SENSOR_NVS_ADDRESS_JOIN_EUI, &setup.join_eui, sizeof(setup.join_eui));
	sensor_nvs_read(SENSOR_NVS_ADDRESS_APP_KEY, &setup.app_key, sizeof(setup.app_key));

	while(is_lorawan_configured(&setup) < 0)
	{
		ret = -1; // set to -1 to indicate that the lorawan is not configured
		LOG_ERR("LoRaWAN is not configured, waiting for configuration");
		k_sleep(K_SECONDS(1));
	}
	if(ret < 0)
	{
		sensor_nvs_write(SENSOR_NVS_ADDRESS_DEV_EUI, &setup.dev_eui, sizeof(setup.dev_eui));
		sensor_nvs_write(SENSOR_NVS_ADDRESS_JOIN_EUI, &setup.join_eui, sizeof(setup.join_eui));
		sensor_nvs_write(SENSOR_NVS_ADDRESS_APP_KEY, &setup.app_key, sizeof(setup.app_key));
	}
	lorawan_log_network_config(&setup);
	ret = sensor_nvs_read(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
	if(ret < 0)
	{
		LOG_INF("No Stored Dev Nonce, using default and storing it in nvs");
		ret = sensor_nvs_write(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
		if(ret < 0)
		{
			LOG_ERR("Failed to store dev nonce in nvs");
		}
	}

	lorawan_setup(&setup);
	/* Store the dev nonce in nvs after finishing join attempts. */
	sensor_nvs_write(SENSOR_NVS_ADDRESS_DEV_NONCE, &setup.dev_nonce, sizeof(setup.dev_nonce));
}

static void alarm_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Alarm callback called");
    if (chan_id == SENSOR_TIMER_CHANNEL_0) {
        LOG_INF("Channel 0 alarm triggered");
    } else if (chan_id == SENSOR_TIMER_CHANNEL_1) {
        LOG_INF("Channel 1 alarm triggered");
    } else if (chan_id == SENSOR_TIMER_CHANNEL_2) {
        LOG_INF("Channel 2 alarm triggered");
    }
}

int main(void)
{
	int ret;
	sensor_timer_alarm_cfg_t alarm_cfg = {
        .callback = alarm_callback,
        .alarm_seconds = MINUTES_TO_SECONDS(2),
        .channel = SENSOR_TIMER_CHANNEL_0,
    };
	sensor_timer_init(timer0);
	ret = sensor_timer_set_alarm(timer0, &alarm_cfg);
	LOG_INF("Number of channels for RTC2: %d", counter_get_num_of_channels(timer0));
	LOG_INF("Frequency for RTC2: %d", counter_get_frequency(timer0));
	LOG_INF("Max top value: %d", counter_get_max_top_value(timer0));
	LOG_INF("Top value: %d", counter_get_top_value(timer0));
	float max_seconds = (float)counter_get_max_top_value(timer0)/counter_get_frequency(timer0);
	LOG_INF("Max seconds: %f", max_seconds);
	LOG_INF("Max Hours: %f", (max_seconds / 3600));
	// init_lora_ble();
	while (1) 
	{
		// send_packet();
		LOG_INF("Timer seconds: %d", sensor_timer_get_current_seconds(timer0));
		LOG_INF("Timer total seconds: %d", sensor_timer_get_total_seconds(timer0));
		k_sleep(K_SECONDS(1));
	}
	return 0;
}
