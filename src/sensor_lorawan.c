#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>

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

	ret = lorawan_start();
	if (ret < 0) {
		return ret;
	}

	int i = 0;
	while (setup->join_attempts == 0 || i < setup->join_attempts) {
		join_cfg.otaa.dev_nonce++;
		ret = lorawan_join(&join_cfg);
		if (ret == 0) {
			break;
		}
		i++;
	}

	if (ret < 0) {
		return ret;
	}

    lorawan_connection_status = 1;
    return 0;
}

int is_lorawan_connected(void) {
	return lorawan_connection_status;
}