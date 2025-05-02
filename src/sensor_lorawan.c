#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>
#include <errno.h>
#include <zephyr/kernel.h>

static int lorawan_connection_status = 0;

/**
 * @brief Check if the lorawan is configured. Returns -1 if the dev_eui, join_eui or app_key are all zeros.	
 * 
 * @param setup lorawan setup struct
 * @return int 0 if configured, -1 if not configured
 */
static int is_lorawan_configured(const lorawan_setup_t *setup) {
    // Helper arrays for comparison
    static const uint8_t zeros_8[8] = {0};
    static const uint8_t zeros_16[16] = {0};
    
    // Check if any of the identifiers are all zeros
    if (memcmp(setup->dev_eui, zeros_8, 8) == 0 ||
        memcmp(setup->join_eui, zeros_8, 8) == 0 ||
        memcmp(setup->app_key, zeros_16, 16) == 0) {
        return -1;
    }
    
    return 0;  // Device is configured
}

int lorawan_setup(lorawan_setup_t *setup)
{
    const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	struct lorawan_join_config join_cfg = {0};
	int ret;
	
	if (is_lorawan_configured(setup) < 0) {
		return -1;
	}
	join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = setup->dev_eui;
	join_cfg.otaa.join_eui = setup->join_eui;
	join_cfg.otaa.app_key = setup->app_key;
	join_cfg.otaa.nwk_key = setup->app_key;
	join_cfg.otaa.dev_nonce = setup->dev_nonce;

	ret = lorawan_start();
	if (ret < 0) {
		return ret;
	}

	ret = lorawan_set_class(setup->uplink_class);
	if (ret < 0) {
		return ret;
	}

	if (setup->downlink_callback.cb != NULL) {
		lorawan_register_downlink_callback(&setup->downlink_callback);
	}

	int i = 0;
	while (setup->join_attempts == 0 || i < setup->join_attempts) {
		join_cfg.otaa.dev_nonce++;
		ret = lorawan_join(&join_cfg);
		if (ret == 0) {
			break;
		}
		k_msleep(setup->delay);
		i++;
	}
	// Save the dev_nonce for the next join
	setup->dev_nonce = join_cfg.otaa.dev_nonce;
	if (ret < 0) {
		return ret;
	}

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
	data->data = NULL;
	data->length = 0;
}

int lorawan_send_data(lorawan_data_t *data)
{
	int ret;
	if (data->length == 0) {
		return -1;
	}

	if(data->attempts == 0)
	{
		ret = lorawan_send(data->port, data->data, data->length, LORAWAN_MSG_UNCONFIRMED);
		reset_data(data);
		return 0;
	}
	else
	{
		for(int i = 0; i < data->attempts; i++)
		{
			ret = lorawan_send(data->port, data->data, data->length, LORAWAN_MSG_CONFIRMED);
	
			if (ret == -EAGAIN) {
				k_msleep(data->delay);
				continue;
			} else if (ret < 0) {
				reset_data(data);
				return(-1);
			} else {
				reset_data(data);
				return 0; // This means ack was received
			}
		}
	}

	reset_data(data);
	return 0;
}


int is_lorawan_connected(void) {
	return lorawan_connection_status;
}