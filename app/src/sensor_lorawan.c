/**
 * @file sensor_lorawan.c
 * @author Tyler Garcia
 * @brief This is a library to handle the LoRaWAN functionality for the sensor.
 * @version 0.1
 * @date 2025-05-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_LORAWAN, LOG_LEVEL_INF);

static int lorawan_connection_status = 0;

/**
 * @brief Check if the lorawan is configured. If it is, setup the join config object.
 * 
 * @param setup lorawan setup struct
 * @param join_cfg lorawan join config struct
 * @return int 0 if configured, -1 if not configured
 */
static int get_lorawan_config(lorawan_setup_t *setup, struct lorawan_join_config *join_cfg)
{
	int ret;
	ret = is_lorawan_configured(setup);
	if (ret < 0) {
		return ret;
	}

	sensor_lorawan_log_network_config(setup);

	join_cfg->mode = LORAWAN_ACT_OTAA;
	join_cfg->dev_eui = setup->dev_eui;
	join_cfg->otaa.join_eui = setup->join_eui;
	join_cfg->otaa.app_key = setup->app_key;
	join_cfg->otaa.nwk_key = setup->app_key;
	join_cfg->otaa.dev_nonce = setup->dev_nonce;
	return 0;  // Device is configured
}

int is_lorawan_configured(lorawan_setup_t *setup)
{
	// Helper arrays for comparison
	static const uint8_t zeros_8[8] = {0};
	static const uint8_t zeros_16[16] = {0};
	// Check if any of the identifiers are all zeros
	if (memcmp(setup->dev_eui, zeros_8, 8) == 0 || memcmp(setup->join_eui, zeros_8, 8) == 0 || memcmp(setup->app_key, zeros_16, 16) == 0) 
	{
		LOG_ERR("LoRaWAN is NOT CONFIGURED");
		return -1;
	}
	LOG_INF("LoRaWAN is CONFIGURED");
	return 0;
}

void sensor_lorawan_log_network_config(lorawan_setup_t *setup)
{
	LOG_INF("DEV EUI:  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		setup->dev_eui[0],setup->dev_eui[1],setup->dev_eui[2],setup->dev_eui[3], \
		setup->dev_eui[4],setup->dev_eui[5],setup->dev_eui[6],setup->dev_eui[7]);
	LOG_INF("JOIN EUI: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		setup->join_eui[0],setup->join_eui[1],setup->join_eui[2],setup->join_eui[3], \
		setup->join_eui[4],setup->join_eui[5],setup->join_eui[6],setup->join_eui[7]);
	LOG_INF("APP KEY:  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		setup->app_key[0],setup->app_key[1],setup->app_key[2],setup->app_key[3], \
		setup->app_key[4],setup->app_key[5],setup->app_key[6],setup->app_key[7], \
		setup->app_key[8],setup->app_key[9],setup->app_key[10],setup->app_key[11], \
		setup->app_key[12],setup->app_key[13],setup->app_key[14],setup->app_key[15]);
}

int sensor_lorawan_setup(lorawan_setup_t *setup)
{
	const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	int ret;
	
	if (!device_is_ready(lora_dev)) {
		LOG_ERR("%s: device not ready.", lora_dev->name);
		return -1;
	}

	struct lorawan_join_config join_cfg;
	if (get_lorawan_config(setup, &join_cfg) < 0) 
	{
		return -1;
	}
	LOG_INF("Starting LoRaWAN");
	ret = lorawan_start();
	if (ret < 0) {
		return ret;
	}
	// Setup downlink callback if there is one
	if (setup->downlink_callback.cb != NULL) {
		lorawan_register_downlink_callback(&setup->downlink_callback);
	}
	// Set the uplink class, default to A
	ret = lorawan_set_class(setup->uplink_class);
	if (ret < 0) {
		LOG_ERR("Failed to set class");
		return ret;
	}

	int i = 0;
	// Join the network, if join_attempts is 0, it will join indefinitely
	while (setup->join_attempts == 0 || i < setup->join_attempts) {
		i++;
		LOG_INF("Join Attempt %d: Dev Nonce: %d", i, join_cfg.otaa.dev_nonce);
		ret = lorawan_join(&join_cfg);
		join_cfg.otaa.dev_nonce++;
		if (ret < 0) {
			if ((ret =-ETIMEDOUT)) {
				LOG_WRN("Timed-out waiting for response.");
			} else {
				LOG_ERR("Join failed (%d)", ret);
			}
		} else {
			LOG_INF("Join successful."); //Changed
			break;
		}
	}
	// Save the dev_nonce for the next join
	setup->dev_nonce = join_cfg.otaa.dev_nonce;
	if (ret < 0) {
		LOG_ERR("Failed to join LoRaWAN");
		return ret;
	}
	LOG_INF("LoRaWAN joined");
	lorawan_connection_status = 1;
	return 0;
}

/**
 * @brief Reset the data to be sent after each lorawan_send_data call.
 * 
 * @param data lorawan data struct
 * @return int 0
 */
static void reset_data(lorawan_data_t *data)
{
	memset(data->data, 0, MAX_LORAWAN_PAYLOAD);
	data->length = 0;
}

static int set_datarate(uint8_t payload_length)
{
	if(payload_length > 125)
	{
		lorawan_set_datarate(LORAWAN_DR_3);
		return 0;
	}
	else if(payload_length > 53)
	{
		lorawan_set_datarate(LORAWAN_DR_2);
		return 0;
	}
	else if(payload_length > 11)
	{
		lorawan_set_datarate(LORAWAN_DR_1);
		return 0;
	}
	else
	{
		lorawan_set_datarate(LORAWAN_DR_0);
		return 0;
	}
}

int sensor_lorawan_send_data(lorawan_data_t *lorawan_data)
{
	int ret;
	if (lorawan_data->length == 0) {
		return -1;
	}
	set_datarate(lorawan_data->length);
	if(lorawan_data->attempts == 0)
	{
		LOG_INF("Sending unconfirmed data");
		ret = lorawan_send(lorawan_data->port, lorawan_data->data, lorawan_data->length, LORAWAN_MSG_UNCONFIRMED);
		reset_data(lorawan_data);
		return 0;
	}
	else
	{
		LOG_INF("Sending Confirmed Data");
		for(int i = 0; i <= lorawan_data->attempts; i++)
		{
			LOG_INF("Attempt %d", i);
			ret = lorawan_send(lorawan_data->port, lorawan_data->data, lorawan_data->length, LORAWAN_MSG_CONFIRMED);
			if (ret < 0) {
				LOG_ERR("Failed to send data with error %d", ret);
				k_msleep(lorawan_data->delay);
				continue;
			} else {
				LOG_INF("Data sent successfully");
				reset_data(lorawan_data);
				return 0; // This means ack was received
			}
		}
	}

	reset_data(lorawan_data);
	if(ret < 0)
	{
		return ret;
	}
	return 0;
}


int is_lorawan_connected(void) {
	return lorawan_connection_status;
}