#include "sensor_lorawan.h"
#include <zephyr/lorawan/lorawan.h>


static int is_lorawan_configured(const lorawan_setup_t *setup) {
    // Helper arrays for comparison
    static const uint8_t zeros_8[8] = {0};
    static const uint8_t zeros_16[16] = {0};
    
    // Check if any of the identifiers are all zeros
    if (memcmp(setup->dev_eui, zeros_8, 8) == 0 ||
        memcmp(setup->join_eui, zeros_8, 8) == 0 ||
        memcmp(setup->app_key, zeros_16, 16) == 0) {
        return -1;  // Device is not configured
    }
    
    return 0;  // Device is configured
}

int lorawan_setup(lorawan_setup_t *setup)
{
    // const struct device *lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	// struct lorawan_join_config join_cfg = {0};
	// int ret;

	// ret = lorawan_start();

	// ret = lorawan_join(&join_cfg);
	if (is_lorawan_configured(setup) < 0) {
		return -1;
	}

    return 0;
}